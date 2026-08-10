//===============================================================================
//
// NeuralNetworkHandler.h
// 
// For the loading and handling of Neural Network Models...
// 
// Used code from : https://dev.to/wolfram27/setting-up-and-using-onnx-runtime-for-c-in-linux-1ho9
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <onnxruntime_cxx_api.h>
#include <juceheader.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>

//===============================================================================
class NeuralNetworkHandler 
{
public:

	NeuralNetworkHandler();
	~NeuralNetworkHandler() = default;

	void loadModel(int modelno);
	void checkModelIO(Ort::Session& session);

	std::vector<float> runModel(std::vector<float>& room,
								std::vector<float>& trajectory,
								std::vector<float>& fir,
								std::vector<float>& beam,
								std::vector<float>& filteredBeam);

private:

	Ort::AllocatorWithDefaultOptions allocator;
	Ort::Env env;
	Ort::SessionOptions session_options;
	std::unique_ptr<Ort::Session> session;

	// Model Paths
	const juce::String model3Path = "BeamNet_ONNX_00003.onnx";
	const juce::String model2Path = "BeamNet_ONNX_00002.onnx";
	const juce::String model1Path = "BeamNet_ONNX_00001.onnx";
};
//===============================================================================