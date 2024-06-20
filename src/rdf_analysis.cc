#include "rdf_analysis/rdf_analysis.h"

#include "ROOT/RDataFrame.hxx"


RDFAnalysis::RDFAnalysis(const fs::path path, const std::string tree_name) {
  node_vec_.emplace_back(RDataFrame{tree_name, path.c_str()});
}

void RDFAnalysis::run(std::function<RNode(RNode&)> action) {
  node_vec_.emplace_back(action(back()));
}

void RDFAnalysis::define(const std::string name, const std::string expr, const bool extend) {
  node_vec_.emplace_back(back().Define(name, expr));
  if (extend) {
    output_branch_vec_.push_back(name);
  }
}

// https://github.com/root-project/root/blob/v6-30-04/tree/dataframe/inc/ROOT/RDF/RInterface.hxx#L337-L341
template <typename F, typename std::enable_if_t<!std::is_convertible<F, std::string>::value, int> >
void RDFAnalysis::define(std::string_view name, F expression, const ColumnNames_t &columns, const bool extend) {
  node_vec_.emplace_back(back().Define(name, std::move(expression), columns));
  if (extend) {
    output_branch_vec_.emplace_back(name);
  }
 }

void RDFAnalysis::alias(const std::string alias, const std::string name, const bool extend) {
  node_vec_.emplace_back(back().Define(alias, name));
  if (extend) {
    output_branch_vec_.push_back(alias);
  }
}

void RDFAnalysis::filter(const std::string expr, const std::string name) {
  node_vec_.emplace_back(back().Filter(expr, name));
}

// https://github.com/root-project/root/blob/v6-30-04/tree/dataframe/inc/ROOT/RDF/RInterface.hxx#L214-L228
template <typename F, std::enable_if_t<!std::is_convertible<F, std::string>::value, int> >
void RDFAnalysis::filter(F f, const ColumnNames_t &columns, std::string_view name) {
  node_vec_.emplace_back(back().Filter(std::move(f), columns, name));
}

RNode& RDFAnalysis::back() {
  return node_vec_.back();
}

void RDFAnalysis::extendOutputBranches(const std::vector<std::string>& branch_vec) {
  // TODO existence
  std::copy(
      branch_vec.begin(),
      branch_vec.end(),
      std::back_inserter(output_branch_vec_));
}

void RDFAnalysis::snapshot(const std::string& path, const std::string& treepath) {
  back().Snapshot(treepath, path, output_branch_vec_);
}

auto RDFAnalysis::report() {
  return back().Report();
}

float RDFAnalysis::sum(const std::string name) {
  return back().Sum(name).GetValue();
}
