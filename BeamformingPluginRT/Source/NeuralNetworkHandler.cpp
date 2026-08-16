//===============================================================================
#include "NeuralNetworkHandler.h"
//===============================================================================
NeuralNetworkHandler::NeuralNetworkHandler() : 
	env(ORT_LOGGING_LEVEL_WARNING, "classifier")
{
	session_options.SetIntraOpNumThreads(1);
	session_options.SetGraphOptimizationLevel(
		GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
}
//===============================================================================
// Checks input/ output sizes and determines successful loading
void NeuralNetworkHandler::checkModelIO(Ort::Session& session)
{
    auto input_name_allocated = session.GetInputNameAllocated(0, allocator);
    auto output_name_allocated = session.GetOutputNameAllocated(0, allocator);

    const char* input_name = input_name_allocated.get();
    const char* output_name = output_name_allocated.get();

    DBG("===========================");
    DBG("Model loaded successfully.");
    DBG("Output name: " + juce::String(output_name));

    for (size_t input = 0; input < session.GetInputCount(); ++input)
    {
        auto input_name_allocated = session.GetInputNameAllocated(input, allocator);
        const char* input_name = input_name_allocated.get();

        auto input_type_info = session.GetInputTypeInfo(input);
        auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
        auto input_shape = input_tensor_info.GetShape();

        juce::String shapeString = "[";

        for (size_t i = 0; i < input_shape.size(); ++i)
        {
            shapeString += juce::String(input_shape[i]);

            if (i + 1 < input_shape.size())
                shapeString += ", ";
        }

        shapeString += "]";

        DBG("Input name: " + juce::String(input_name));
        DBG("Input shape: " + shapeString);
    }
    DBG("===========================");
}
//===============================================================================
void NeuralNetworkHandler::loadModel(int modelno)
{
    try
    {
        // Get the file using JUCE
        juce::File modelFolder =
            juce::File::getSpecialLocation(
                juce::File::currentExecutableFile)
            .getParentDirectory()
            .getChildFile("Models");

        juce::File modelFile;

        switch (modelno)
        {
        case 1:
            modelFile = modelFolder.getChildFile(model1Path);
            break;

        case 2:
            modelFile = modelFolder.getChildFile(model2Path);
            break;

        case 3:
            modelFile = modelFolder.getChildFile(model3Path);
            break;

        default:
            modelFile = modelFolder.getChildFile(model3Path);
            break;
        }

        session = std::make_unique<Ort::Session>(env, modelFile.getFullPathName().toWideCharPointer(), session_options);

        // Uses I/O cheack function
        checkModelIO(*session);
    }
    catch (const Ort::Exception& e)
    {
        DBG("ONNX Runtime error: " + juce::String(e.what()));
    }
}
//===============================================================================
// Creates tensors from input and passes to model
std::vector<float> NeuralNetworkHandler::runModel(std::vector<float>& room,
                                                  std::vector<float>& trajectory,
                                                  std::vector<float>& fir,
                                                  std::vector<float>& beam,
                                                  std::vector<float>& filteredBeam)
{
    Ort::MemoryInfo memory_info =
        Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    // Define tensor shapes form model
    std::vector<int64_t> room_shape = { 1, 7 };
    std::vector<int64_t> trajectory_shape = { 1, 35 };
    std::vector<int64_t> fir_shape = { 1, 1, 100, 8, 64 };
    std::vector<int64_t> beam_shape = { 1, 1, 72, 100 };
    std::vector<int64_t> filtered_beam_shape = { 1, 1, 72, 100 };
    
    // Define Input Tensors
    Ort::Value room_tensor = Ort::Value::CreateTensor<float>(
        memory_info,
        const_cast<float*>(room.data()),
        room.size(),
        room_shape.data(),
        room_shape.size());

    Ort::Value trajectory_tensor = Ort::Value::CreateTensor<float>(
        memory_info,
        const_cast<float*>(trajectory.data()),
        trajectory.size(),
        trajectory_shape.data(),
        trajectory_shape.size());

    Ort::Value fir_tensor = Ort::Value::CreateTensor<float>(
        memory_info,
        const_cast<float*>(fir.data()),
        fir.size(),
        fir_shape.data(),
        fir_shape.size());

    Ort::Value beam_tensor = Ort::Value::CreateTensor<float>(
        memory_info,
        const_cast<float*>(beam.data()),
        beam.size(),
        beam_shape.data(),
        beam_shape.size());

    Ort::Value filtered_beam_tensor = Ort::Value::CreateTensor<float>(
        memory_info,
        const_cast<float*>(filteredBeam.data()),
        filteredBeam.size(),
        filtered_beam_shape.data(),
        filtered_beam_shape.size());

    // I/O names
    const char* input_names[] = {"room", "trajectory", "fir", "beam", "filtered_beam"};
    const char* output_names[] = { "output" };

    Ort::Value input_tensors[] = {
        std::move(room_tensor),
        std::move(trajectory_tensor),
        std::move(fir_tensor),
        std::move(beam_tensor),
        std::move(filtered_beam_tensor)
    };

    auto output_tensors = session->Run(
        Ort::RunOptions{ nullptr },
        input_names,
        input_tensors,
        5,
        output_names,
        1);

    // Get Data from output tensor
    float* output_data = output_tensors[0].GetTensorMutableData<float>();

    // Get Shape of output tensor
    auto output_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();

    size_t output_size = 1;

    for (auto dim : output_shape) output_size *= static_cast<size_t>(dim);

    // Return Output Data
    return std::vector<float>(output_data, output_data + output_size);
}
//===============================================================================