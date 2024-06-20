#ifndef RDFANALYSIS_RDFANALYSIS_H_
#define RDFANALYSIS_RDFANALYSIS_H_

#include "ROOT/RDataFrame.hxx"

#include <functional>
#include <string>
#include <vector>

#include <filesystem>

using namespace ROOT;
using namespace ROOT::RDF;
namespace fs = std::filesystem;


class RDFAnalysis {
public:

  RDFAnalysis(const fs::path path, const std::string tree_name);

  void run(std::function<RNode(RNode&)> action);

  void define(const std::string name, const std::string expr, const bool extend = false);

  // https://github.com/root-project/root/blob/v6-30-04/tree/dataframe/inc/ROOT/RDF/RInterface.hxx#L337-L341
  template <typename F, typename std::enable_if_t<!std::is_convertible<F, std::string>::value, int> = 0>
  void define(std::string_view name, F expression, const ColumnNames_t &columns = {}, const bool extend = false);

  void alias(const std::string alias, const std::string name, const bool extend = false);

  void filter(const std::string expr, const std::string name);

  // https://github.com/root-project/root/blob/v6-30-04/tree/dataframe/inc/ROOT/RDF/RInterface.hxx#L214-L228
  template <typename F, std::enable_if_t<!std::is_convertible<F, std::string>::value, int> = 0>
  void filter(F f, const ColumnNames_t &columns = {}, std::string_view name = "");

  RNode& back();

  void extendOutputBranches(const std::vector<std::string>& branch_vec);

  void snapshot(const std::string& path, const std::string& treepath);

  auto report();

  float sum(const std::string name);

private:
  std::vector<RNode> node_vec_;
  std::vector<std::string> output_branch_vec_;
};

#endif // ANALYSIS_RDFANALYSIS_H_
