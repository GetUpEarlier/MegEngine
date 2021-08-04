/**
 * \file src/mge/common.h
 *
 * This file is part of MegEngine, a deep learning framework developed by
 * Megvii.
 *
 * \copyright Copyright (c) 2020-2021 Megvii Inc. All rights reserved.
 */

#pragma once

#include "lite_build_config.h"

#if LITE_BUILD_WITH_MGE
#include "../misc.h"
#include "lite/network.h"
#include "lite/tensor.h"
#include "megbrain/comp_node.h"
#include "megbrain/serialization/serializer.h"
#include "megbrain/tensor.h"

//! rename mge name L*
namespace lite {
using LTensorLayout = mgb::TensorLayout;
using LComputingGraph = mgb::ComputingGraph;
using LDeviceTensorStorage = mgb::DeviceTensorStorage;
}  // namespace lite

namespace lite {
/*!
 * \brief transform mgelite Layout to mgb TensorLayout
 */
LTensorLayout to_impl_layout(const Layout& layout);

/*!
 * \brief transform mgb TensorLayout to mgelite Layout
 */
Layout to_lite_layout(const mgb::TensorLayout& mge_layout);

/*!
 * \brief transform mgelite device to mgb CompNode Locator
 */
mgb::CompNode::Locator to_compnode_locator(const LiteDeviceType& device);

/*!
 * \brief transform mgb CompNode Locator to lite Device
 */
LiteDeviceType get_device_from_locator(const mgb::CompNode::Locator& locator);

/*! \brief A megbrain tensor loader with weight decompression.
 *
 * The weight to be compressed must start with a byte of compression flag (CF).
 *
 * 1. CF = 0: no compression.
 * 2. CF = 1: float32 stride + float32 base + uint8 weight (return s*w+b)
 * 3. CF = 2: float32 stride + float32 base + uint16 weight (return s*w+b)
 *
 */
void decompressed_tensor_value_loader(void* ptr_,
                                      const mgb::TensorLayout& layout,
                                      mgb::serialization::InputFile& fin);

}  // namespace lite
#endif

// vim: syntax=cpp.doxygen foldmethod=marker foldmarker=f{{{,f}}}
