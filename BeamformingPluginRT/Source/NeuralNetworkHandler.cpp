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
