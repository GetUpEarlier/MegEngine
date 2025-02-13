/**
 * \file lite/load_and_run/src/options/io_options.h
 *
 * This file is part of MegEngine, a deep learning framework developed by
 * Megvii.
 *
 * \copyright Copyright (c) 2020-2021 Megvii Inc. All rights reserved.
 */

#pragma once
#include <gflags/gflags.h>
#include "helpers/outdumper.h"
#include "megbrain/plugin/opr_io_dump.h"
#include "models/model.h"
#include "option_base.h"

DECLARE_string(input);

DECLARE_string(io_dump);
DECLARE_bool(io_dump_stdout);
DECLARE_bool(io_dump_stderr);
DECLARE_string(bin_io_dump);
DECLARE_string(bin_out_dump);
DECLARE_bool(copy_to_host);

namespace lar {

/*!
 * \brief: input option for --input set
 */
class InputOption final : public OptionBase {
public:
    //! static function for registe options
    static bool is_valid() { return !FLAGS_input.empty(); };
    static std::shared_ptr<OptionBase> create_option();

    void config_model(
            RuntimeParam& runtime_param, std::shared_ptr<ModelBase> model) override;
    //! interface implement from OptionBase
    std::string option_name() const override { return m_option_name; };

private:
    InputOption();

    template <typename ModelImpl>
    void config_model_internel(RuntimeParam&, std::shared_ptr<ModelImpl>){};

    std::string m_option_name;
    std::vector<std::string> data_path;  // data string or data file path
};

class IOdumpOption : public OptionBase {
public:
    static bool is_valid();
    static std::shared_ptr<OptionBase> create_option();
    //! config the model, if different has different configure code, then
    //! dispatch
    void config_model(
            RuntimeParam& runtime_param, std::shared_ptr<ModelBase> model) override;
    std::string option_name() const override { return m_option_name; };

private:
    IOdumpOption();
    template <typename ModelImpl>
    void config_model_internel(RuntimeParam&, std::shared_ptr<ModelImpl>){};

    bool enable_io_dump;
    bool enable_io_dump_stdout;
    bool enable_io_dump_stderr;
    bool enable_bin_io_dump;
    bool enable_bin_out_dump;
    bool enable_copy_to_host;
    std::string m_option_name;
    std::string dump_path;
    std::unique_ptr<mgb::OprIODumpBase> io_dumper;
    std::unique_ptr<OutputDumper> out_dumper;
};
}  // namespace lar
