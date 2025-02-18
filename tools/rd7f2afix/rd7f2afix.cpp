#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " input_file path\n";
    return 1;
  }

  std::string input_filename =
      std::filesystem::path(argv[1]).filename().string();
  std::string input_path = argv[1];
  std::string path = argv[2];
  std::string header_filename =
      input_filename.substr(0, input_filename.find_last_of(".")) + ".hpp";
  std::string include_statement =
      "#include <" + path + "/" + header_filename + ">";

  std::ifstream input_file(input_path);
  if (!input_file.is_open()) {
    std::cerr << "Error: Failed to open input file\n";
    return 1;
  }
  std::string input_contents((std::istreambuf_iterator<char>(input_file)),
                             std::istreambuf_iterator<char>());
  input_file.close();

  std::regex pattern("#include\\s+<.*\\.hpp>\\s*?");
  input_contents =
      std::regex_replace(input_contents, pattern, include_statement);

  std::ofstream output_file(input_filename);
  if (!output_file.is_open()) {
    std::cerr << "Error: Failed to open output file for writing\n";
    return 1;
  }
  output_file << input_contents;
  output_file.close();

  std::cout << "Done! Output written to " << input_filename << std::endl;
  std::cout << "Include statement: " << include_statement << std::endl;
  return 0;
}