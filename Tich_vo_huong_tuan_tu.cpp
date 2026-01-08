#include <iostream>
#include <vector>
#include <chrono> // Thư viện đo thời gian chính xác của C++ chuẩn

using namespace std;
using namespace std::chrono;

int main() {
    long N = 10000000000;
    vector<double> A(N, 1.0);
    vector<double> B(N, 2.0);
    double sum = 0.0;
    auto start_time = high_resolution_clock::now();
    for (long i = 0; i < N; ++i) {
        sum += A[i] * B[i];
    }
    auto end_time = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(end_time - start_time);

    // 3. XUẤT KẾT QUẢ
    cout << "---------------------------------" << endl;
    cout << "Ket qua (Tich vo huong): " << sum << endl;
    cout << "Thoi gian thuc thi     : " << time_span.count() << " giay" << endl;
    cout << "---------------------------------" << endl;

    return 0;
}
