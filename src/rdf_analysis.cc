#include "rdf_analysis/rdf_analysis.h"

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include <cmath>
#include <cstdint>


RDFAnalysis::RDFAnalysis(
    const fs::path path,
    const std::string tree_name,
    const bool do_cutflow,
    const bool add_progress_bar)
    : do_cutflow_(do_cutflow){
  node_vec_.emplace_back(RDataFrame{tree_name, path.c_str()});

  if (add_progress_bar) {
    ROOT::RDF::Experimental::AddProgressBar(back());
  }
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

void RDFAnalysis::alias(const std::string alias, const std::string name, const bool extend) {
  node_vec_.emplace_back(back().Define(alias, name));
  if (extend) {
    output_branch_vec_.push_back(alias);
  }
}

void RDFAnalysis::filter(const std::string expr, const std::string name) {
  if (do_cutflow_ and cutflow_.empty()) {
    cutflow_.emplace_back("total", sum("weight"));
  }

  node_vec_.emplace_back(back().Filter(expr, name));

  if (do_cutflow_) {
    cutflow_.emplace_back(name, sum("weight"));
  }
}

void RDFAnalysis::filter(const std::string expr) {
  filter(expr, expr);
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

  if (do_cutflow_) {
    TH1D* h_cutflow = makeCutflowHist();
    auto output_file = TFile::Open(path.c_str(), "UPDATE");
    h_cutflow->SetDirectory(output_file);
    output_file->Write();
    output_file->Close();
  }
}

auto RDFAnalysis::report() {
  return back().Report();
}

float RDFAnalysis::sum(const std::string name) {
  return back().Sum(name).GetValue();
}


TH1D* RDFAnalysis::makeCutflowHist() {
  const int32_t nbinsx = cutflow_.size();
  const double_t xlow = -0.5;
  const double_t xup = cutflow_.size() - 0.5;

  auto h_cutflow = new TH1D{"cutflow", "", nbinsx, xlow, xup};
  for (size_t idx = 0; idx < cutflow_.size(); idx++) {
    const auto [key, value] = cutflow_.at(idx);

    h_cutflow->Fill(idx, value);
    h_cutflow->GetXaxis()->SetBinLabel(idx + 1, key.c_str());
  }

  h_cutflow->GetYaxis()->SetTitle("Sum of Event Weights");

  return h_cutflow;
}
