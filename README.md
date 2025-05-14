# Tên Game: Fruit Ninja

## Thông tin sinh viên
- **Tên sinh viên**: Trần Cẩm Sơn
- **Mã sinh viên**: 24021615

## Giới thiệu chủ đề game
Fruit Ninja là một tựa game hành động arcade nổi tiếng, nơi người chơi sử dụng chuột để cắt các loại trái cây xuất hiện trên màn hình, đồng thời tránh chạm vào bom để không thua cuộc. Game được phát triển bằng ngôn ngữ lập trình C/C++ và sử dụng thư viện SDL2 để xử lý đồ họa, âm thanh và sự kiện. Mục tiêu là tạo ra một trải nghiệm mượt mà, vui nhộn, tái hiện tinh thần của phiên bản gốc với các cải tiến cá nhân.

## Ngưỡng điểm mong muốn
Tôi mong muốn đạt được ngưỡng điểm 8-9.

## Lý do bảo vệ ngưỡng điểm
Tôi tin rằng mình xứng đáng với ngưỡng điểm này dựa trên các yếu tố sau:
*Tính năng đã thực hiện*
**Hệ thống cắt trái cây**:
- Người chơi có thể dùng chuột để "cắt" trái cây bằng cách kéo con trỏ qua các đối tượng trên màn hình.
Hiệu ứng cắt được hiển thị với các vệt sáng và âm thanh chân thực.
**Đối tượng trò chơi**:
- Trái cây (táo, chuối, dưa hấu, v.v.) xuất hiện ngẫu nhiên với quỹ đạo ném parabol. Bom xuất hiện xen kẽ, nếu cắt trúng sẽ trừ điểm hoặc kết thúc trò chơi.
**Giao diện và đồ họa**:
- Màn hình chơi hiển thị điểm số, số mạng (lives), và thời gian. Hiệu ứng hình ảnh như trái cây bị cắt đôi, nước trái cây bắn ra.
- Âm thanh: Âm thanh vui nhộn phù hợp với không khí game. Hiệu ứng âm thanh khi cắt trái cây, cắt trúng bom, hoặc kết thúc trò chơi.
**Cách thức chơi**:
Người chơi cắt trái cây để đạt điểm cao nhất, mất mạng khi bỏ lỡ trái cây hoặc cắt trúng bom.

## Mức độ phụ thuộc AI

Mức độ phụ thuộc AI: 20%
Sử dụng AI để tham khảo thêm về các câu lệnh chi tiết của SDL2 và hỗ trợ xử lý thuật toán tung trái cây.

## Thuật toán đã cài đặt

**Phát hiện va chạm**:
- Sử dụng thuật toán kiểm tra giao điểm giữa đường di chuyển của con trỏ chuột (dạng đường thẳng) và vùng bao quanh trái cây/bom (hình tròn hoặc hình chữ nhật).
Tối ưu hóa bằng cách giảm số lần kiểm tra va chạm thông qua phân vùng không gian.
**Quỹ đạo chuyển động**:
- Áp dụng công thức chuyển động parabol để mô phỏng quỹ đạo ném của trái cây, tính toán dựa trên vận tốc ban đầu, góc ném, và gia tốc trọng trường.
- Bom có quỹ đạo tương tự nhưng với tần suất xuất hiện ngẫu nhiên.
**Tạo đối tượng ngẫu nhiên**:
- Sử dụng hàm rand() để tạo vị trí xuất hiện, loại trái cây, và thời gian xuất hiện ngẫu nhiên, đảm bảo tính đa dạng và thử thách.
**Quản lý sự kiện**:
- Xử lý sự kiện chuột (di chuyển, nhấn) bằng SDL2 event loop.
- Tối ưu hóa để giảm độ trễ khi xử lý input.

## Nguồn tham khảo
Dưới đây là các nguồn tham khảo tôi đã sử dụng trong quá trình phát triển game:

- Tài liệu SDL2 chính thức: https://wiki.libsdl.org/
- Hướng dẫn lập trình game với SDL2: Lazy Foo' Productions
- Tài liệu về thuật toán phát hiện va chạm: GameDev.net
- Mã nguồn tham khảo trên GitHub: Các dự án SDL2 open-source.
- Hướng dẫn Git và GitHub: GitHub Docs

## Yêu cầu hệ thống
- Hệ điều hành: Windows, macOS, hoặc Linux.
- Trình biên dịch: GCC (Linux/macOS) hoặc MinGW (Windows).
- Thư viện SDL2 đã được cài đặt.

## Hướng dẫn chạy game
1. Tải file đóng gói từ https://github.com/transonnn/Aim_Master.git
2. Giải nén file `.zip` vào một thư mục trên máy tính của bạn.
3. Nhấp đúp vào file `game.exe` để chạy game.

