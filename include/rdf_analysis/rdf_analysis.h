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

  RDFAnalysis(const fs::path path,
              const std::string tree_name,
              const bool do_cutflow = true,
              const bool add_progress_bar = false);

  void run(std::function<RNode(RNode&)> action);

  void define(const std::string name, const std::string expr, const bool extend = false);

  // https://github.com/root-project/root/blob/v6-30-04/tree/dataframe/inc/ROOT/RDF/RInterface.hxx#L337-L341
  template <typename F, typename std::enable_if_t<!std::is_convertible<F, std::string>::value, int> = 0>
  void define(std::string_view name, F expression, const ColumnNames_t &columns = {}, const bool extend = false) {
    node_vec_.emplace_back(back().Define(name, std::move(expression), columns));
    if (extend) {
      output_branch_vec_.emplace_back(name);
    }
   }

  void alias(const std::string alias, const std::string name, const bool extend = false);

  void filter(const std::string expr, const std::string name);

  // name is same with expr
  void filter(const std::string expr);

  // https://github.com/root-project/root/blob/v6-30-04/tree/dataframe/inc/ROOT/RDF/RInterface.hxx#L214-L228
  template <typename F, std::enable_if_t<!std::is_convertible<F, std::string>::value, int> = 0>
  void filter(F f, const ColumnNames_t &columns = {}, std::string_view name = "") {
    if (do_cutflow_ and cutflow_.empty()) {
      cutflow_.emplace_back("total", sum("weight"));
    }

    node_vec_.emplace_back(back().Filter(std::move(f), columns, name));

    if (do_cutflow_) {
      cutflow_.emplace_back(name, sum("weight"));
    }
  }

  RNode& back();

  void extendOutputBranches(const std::vector<std::string>& branch_vec);

  void snapshot(const std::string& path, const std::string& treepath);

  auto report();

  float sum(const std::string name);


  TH1D* makeCutflowHist();

private:
  const bool do_cutflow_;
  std::vector<RNode> node_vec_;
  std::vector<std::string> output_branch_vec_;
  std::vector<std::pair<std::string, float> > cutflow_;
};

#endif // ANALYSIS_RDFANALYSIS_H_
