#ifndef UTILS_FILE_DESCRIPTOR_HPP
#define UTILS_FILE_DESCRIPTOR_HPP


namespace utils {

class FileDescriptor {
public:
    FileDescriptor() = default;
    FileDescriptor(int fd) noexcept;
    FileDescriptor(FileDescriptor&& other) noexcept;
    ~FileDescriptor() noexcept;

    FileDescriptor(const FileDescriptor& other) = delete;

    FileDescriptor& operator=(int fd);

    operator int() const noexcept;
    void close();
    int extract();

    bool is_opened() const noexcept;

private:
    int m_fd = -1;
};

} // namespace utils

#endif // UTILS_FILE_DESCRIPTOR_HPP