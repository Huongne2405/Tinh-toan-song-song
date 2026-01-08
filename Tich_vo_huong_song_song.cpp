#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {
    long N = 100000000;
    vector<double> A(N, 1.0);
    vector<double> B(N, 2.0);
    double sum = 0.0;
    int requested_threads;
    int max_possible = omp_get_max_threads(); // Lấy số luồng tối đa máy hỗ trợ

    cout << "May tinh cua ban ho tro toi da: " << max_possible << " luong." << endl;
    cout << "Nhap so luong muon chay: ";
    cin >> requested_threads;

    // Kiểm tra hợp lệ
    if (requested_threads <= 0) requested_threads = 1;
    omp_set_num_threads(requested_threads); // Số luồng sử dụng
    cout << "DANG CHAY VOI " << requested_threads << " LUONG" << endl;
    auto start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < N; ++i) {
        sum += A[i] * B[i];
    }
    auto end = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(end - start);
    cout << fixed << setprecision(5);
    cout << "Ket qua: " << sum << endl;
    cout << "Thoi gian: " << time_span.count() << " giay" << endl;
    return 0;
}
