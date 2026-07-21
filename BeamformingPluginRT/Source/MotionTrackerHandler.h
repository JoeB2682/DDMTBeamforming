//===============================================================================
//
// MotionTrackerHandler.h
// 
// Uses NatNet SDK to link to the Optitrack motion tracking system. Needs to have
// Optitrack system with Motive software running. Data Streaming MUST be enabled 
// inside of Motive. 
// 
// Link to NatNet SDK source:
// 
// https://www.optitrack.com/support/downloads
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <NatNetTypes.h>
#include <NatNetCAPI.h>
#include <NatNetClient.h>
#include <atomic>
#include <JuceHeader.h>

//===============================================================================
// Structure to store x, y, z, coordinates
struct Position { float x, y, z; };
//===============================================================================
class MotionTrackerHandler 
{
public:

	MotionTrackerHandler();
	~MotionTrackerHandler();

	// Connect to Motive via local IP
	void connect();
	
	// Get x, y, z coordinates
	inline Position getPosition() { return position; }

private:

	// Callback for when motion tracking frame arrives from Motive
	static void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData);

	NatNetClient* client;

public:

	// Atomic ensures safe access when multithreadingso
	std::atomic<float> x{ 0.0f };
	std::atomic<float> y{ 0.0f };
	std::atomic<float> z{ 0.0f };

	Position position;
};
//===============================================================================