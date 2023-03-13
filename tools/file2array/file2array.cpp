#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage:\nfile2array <file> <dstfilename>\n";
    return 0;
  }
  std::string file_path = argv[1];
  std::ifstream file(file_path, std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "[-] Could not open file" << std::endl;
    return 1;
  }

  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

  const size_t array_size = buffer.size();
  std::ofstream fout(std::string(argv[2]) + ".cpp");
  fout << "#include <" << argv[2] << ".hpp>" << std::endl;
  fout << "\nunsigned char " << argv[2] << "[] = {";
  for (std::size_t i = 0; i < buffer.size(); ++i) {
    if (i != 0) {
      fout << ", ";
    }
    fout << "0x" << std::hex << static_cast<int>(buffer[i]);
  }
  fout << "};" << std::endl;

  fout << "\nsize_t " << argv[2] << "_size = 0x" << std::hex << array_size
       << ";" << std::endl;

  std::ofstream fhead(std::string(argv[2]) + ".hpp");
  fhead << "#pragma once\n#include <cstddef>\n";
  fhead << "\nextern unsigned char " << argv[2] << "[];";
  fhead << "\nextern size_t " << argv[2] << "_size;";

  file.close();
  fout.close();
  return 0;
}