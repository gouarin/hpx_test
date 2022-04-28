#include <vector>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/util.hpp>

#include "iterator.hpp"
#include "update.hpp"

///////////////////////////////////////////////////////////////////////////////
// Command-line variables
bool header = true; // print csv heading

int hpx_main(hpx::program_options::variables_map& vm)
{
    std::uint64_t nx = vm["nx"].as<std::uint64_t>();   // Number of grid points.
    std::uint64_t ny = vm["ny"].as<std::uint64_t>();   // Number of grid points.
    std::uint64_t nt = vm["nt"].as<std::uint64_t>();   // Number of steps.

    if (vm.count("no-header"))
        header = false;

    using allocator_type = hpx::compute::host::block_allocator<double>;
    using executor_type = hpx::compute::host::block_executor<>;
    using data_type = hpx::compute::vector<double, allocator_type>;
    using iterator = row_iterator<typename data_type::iterator>;

    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);

    std::array<data_type, 2> U;

    U[0] = data_type(nx * ny, 1.0, alloc);
    U[1] = data_type(nx * ny, 1.0, alloc);
    // init(U, nx, ny);

    // Construct our column iterators. We want to begin with the second
    // row to avoid out of bound accesses.
    iterator curr(nx, U[0].begin());
    iterator next(nx, U[1].begin());

    executor_type executor(numa_domains);
    auto policy = hpx::execution::par.on(executor);

    hpx::chrono::high_resolution_timer t;
    for (std::size_t t = 0; t < nt; ++t)
    {
        // We store the result of our update in the next middle line.
        // Iterate over the interior: skip the first and last column
        hpx::for_loop(policy,
            curr + 1, curr + ny - 1,
            // We need to advance the result by one row each iteration
            hpx::parallel::induction(next.middle_ + nx, nx),
            [nx](iterator it, data_type::iterator result)
            {
                line_update(*it, *it + nx, result);
            }
        );

        std::swap(curr, next);
    }

    double elapsed = t.elapsed();
    double mlups = ((nx -2)*(ny - 2)*nt)/1e6/elapsed;
    std::cout << "MLUPS " << mlups << std::endl;

    if (vm.count("results"))
    {
        for(std::size_t j = 0; j < ny; ++j)
        {
            for(std::size_t i = 0; i < nx; ++i)
            {
                std::cout << U[1][j*nx + i] << " ";
            }
            std::cout << std::endl;
        }
    }
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    namespace po = hpx::program_options;

    po::options_description desc_commandline;
    desc_commandline.add_options()
        ("results", "print generated results (default: false)")
        ("nx", po::value<std::uint64_t>()->default_value(100),
         "Local x dimension")
        ("ny", po::value<std::uint64_t>()->default_value(100),
         "Local y dimension")
        ("nt", po::value<std::uint64_t>()->default_value(45),
         "Number of time steps")
        ( "no-header", "do not print out the csv header row")
    ;

    // Initialize and run HPX
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args);
}
