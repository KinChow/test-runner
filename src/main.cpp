#include <vector>
#include "elementwise.h"
#include "log.h"
#include "test_runner.h"

int main(int argc, char *argv[])
{
    Benchmark benchmark(10, 100);
    constexpr size_t kSize = 1024 * 1024 * 32;
    std::vector<float> baseline_data_a(kSize);
    std::vector<float> baseline_data_b(kSize);
    std::vector<float> baseline_result(kSize);
    std::vector<float> comparison_data_a(kSize);
    std::vector<float> comparison_data_b(kSize);
    std::vector<float> comparison_result(kSize);
    benchmark.AddBaselineTask("ElementwiseAdd", ElementwiseAdd, baseline_data_a.data(), baseline_data_b.data(),
        baseline_result.data(), kSize);
    benchmark.AddComparisonTask("ElementwiseAddUnroll", ElementwiseAddUnroll, comparison_data_a.data(),
        comparison_data_b.data(), comparison_result.data(), kSize);
    if (!benchmark.Run()) {
        LOGE("Benchmark failed to run.");
        return 1;
    }
    LOGI("\n%s\n", benchmark.GetResultString().c_str());
    LOGI("Benchmark completed successfully.");
    return 0;
}