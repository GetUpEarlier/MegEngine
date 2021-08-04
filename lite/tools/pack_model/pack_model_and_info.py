#!/usr/bin/python3
# -*- coding: utf-8 -*-
#
# This file is part of MegEngine, a deep learning framework developed by
# Megvii.
#
# copyright Copyright (c) 2020-2021 Megvii Inc. All rights reserved.

import argparse
import struct
import os
import subprocess

import flatbuffers

def generate_flatbuffer():
    status, path = subprocess.getstatusoutput('which flatc')
    if not status:
        cwd = os.path.dirname(os.path.dirname(__file__))
        fbs_file = os.path.abspath(os.path.join(cwd,
            "../../src/parse_model/pack_model.fbs"))
        cmd = path + ' -p -b '+fbs_file
        ret, _ = subprocess.getstatusoutput(str(cmd))
        if ret:
            raise Exception("flatc generate error!")
    else:
        raise Exception('no flatc in current environment, please build flatc '
                'and put in the system PATH!')

def main():
    parser = argparse.ArgumentParser(
            description='load a encrypted or not encrypted model and a '
            'json format of the infomation of the model, pack them to a file '
            'which can be loaded by lite.')
    parser.add_argument('--input-model', help='input a encrypted or not encrypted model')
    parser.add_argument('--input-info', help='input a encrypted or not encrypted '
            'json format file.')
    parser.add_argument('--model-name', help='the model name, this must match '
            'with the model name in model info', default = 'NONE')
    parser.add_argument('--model-cryption', help='the model encryption method '
            'name, this is used to find the right decryption method. e.g. '
            '--model_cryption = "AES_default", default is NONE.', default =
            'NONE')
    parser.add_argument('--info-cryption', help='the info encryption method '
            'name, this is used to find the right decryption method. e.g. '
            '--model_cryption = "AES_default", default is NONE.', default =
            'NONE')
    parser.add_argument('--info-parser', help='The information parse method name '
            'default is "LITE_default". ', default = 'LITE_default')
    parser.add_argument('--append', '-a', help='append another model to a '
            'packed model.')
    parser.add_argument('--output', '-o', help='output file of packed model.')

    args = parser.parse_args()

    generate_flatbuffer()
    assert not args.append, ('--append is not support yet')
    assert args.input_model, ('--input_model must be given')
    with open(args.input_model, 'rb') as fin:
        raw_model = fin.read()

    model_length = len(raw_model)

    if args.input_info:
        with open(args.input_info, 'rb') as fin:
            raw_info = fin.read()
            info_length = len(raw_info)
    else:
        raw_info = None
        info_length = 0

    # Generated by `flatc`.
    from model_parse import Model, ModelData, ModelHeader, ModelInfo, PackModel

    builder = flatbuffers.Builder(1024)

    model_name = builder.CreateString(args.model_name)
    model_cryption = builder.CreateString(args.model_cryption)
    info_cryption = builder.CreateString(args.info_cryption)
    info_parser = builder.CreateString(args.info_parser)

    info_data = builder.CreateByteVector(raw_info)
    arr_data = builder.CreateByteVector(raw_model)

    #model header
    ModelHeader.ModelHeaderStart(builder)
    ModelHeader.ModelHeaderAddName(builder, model_name)
    ModelHeader.ModelHeaderAddModelDecryptionMethod(builder, model_cryption)
    ModelHeader.ModelHeaderAddInfoDecryptionMethod(builder, info_cryption)
    ModelHeader.ModelHeaderAddInfoParseMethod(builder, info_parser)
    model_header = ModelHeader.ModelHeaderEnd(builder)

    #model info
    ModelInfo.ModelInfoStart(builder)
    ModelInfo.ModelInfoAddData(builder, info_data)
    model_info = ModelInfo.ModelInfoEnd(builder)

    #model data
    ModelData.ModelDataStart(builder)
    ModelData.ModelDataAddData(builder, arr_data)
    model_data = ModelData.ModelDataEnd(builder)

    Model.ModelStart(builder)
    Model.ModelAddHeader(builder, model_header)
    Model.ModelAddData(builder, model_data)
    Model.ModelAddInfo(builder, model_info)
    model = Model.ModelEnd(builder)

    PackModel.PackModelStartModelsVector(builder, 1)
    builder.PrependUOffsetTRelative(model)
    models = builder.EndVector(1)

    PackModel.PackModelStart(builder)
    PackModel.PackModelAddModels(builder, models)
    packed_model = PackModel.PackModelEnd(builder)

    builder.Finish(packed_model)
    buff = builder.Output()

    result = struct.pack(str(len("packed_model")) + 's', "packed_model".encode('ascii'))
    result += buff

    assert args.output, ('--output must be given')
    with open(args.output, 'wb') as fin:
        fin.write(result)

    print("Model packaged successfully!!!")
    print("model name is: {}.".format(args.model_name))
    print("model encryption method is: {}. ".format(args.model_cryption))
    print("model json infomation encryption method is: {}. ".format(args.info_cryption))
    print("model json infomation parse method is: {}. ".format(args.info_parser))
    print("packed model is write to {} ".format(args.output))

if __name__ == '__main__':
    main()
