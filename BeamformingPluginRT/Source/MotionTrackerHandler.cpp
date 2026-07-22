//===============================================================================
#include "MotionTrackerHandler.h"
//===============================================================================
// Constructor and Destructor
MotionTrackerHandler::MotionTrackerHandler() 
{
	// Client connects to NatNet server and handles motiv data
	client = new NatNetClient(ConnectionType_Unicast);

	client->SetDataCallback(DataHandler, this);

	int callbackResult = client->SetDataCallback(DataHandler, this);

	DBG("Callback result: " << callbackResult);

	// initialise position struct
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
}

MotionTrackerHandler::~MotionTrackerHandler() 
{
	// Disconnect and deallopcate memory if the clients still alive
	if (client) 
	{
		client->Uninitialize();

		delete client;
		client = nullptr;
	}
}
//===============================================================================
// Connect to Motive on local serevr
void MotionTrackerHandler::connect()
{
	int result = client->Initialize(
		"192.168.56.1",
		"192.168.56.1"
	);

	DBG("Connect result: " << result);

	sFrameOfMocapData* frame = client->GetLastFrameOfData();

	/*
	if (frame)
		DBG("Frame exists");
	else
		DBG("No frame");
		*/
}
//===============================================================================
// Get motion tracking data and store
void MotionTrackerHandler::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	// Create pointer to current object
	auto* tracker = static_cast<MotionTrackerHandler*>(pUserData);

	//DBG("Frame received");
	//DBG("Rigid bodies: " << data->nRigidBodies);

	// Loop through rigidbody data
	if (data->nRigidBodies > 0)
	{
		// Get data point
		auto& rb = data->RigidBodies[0];

		//DBG("RB: " << rb.x << " " << rb.y << " " << rb.z);

		// store data points
		tracker->x.store(rb.x);
		tracker->y.store(rb.y);
		tracker->z.store(rb.z);
		
		// Update position struct
		tracker->position.x = tracker->x.load();
		tracker->position.y = tracker->y.load();
		tracker->position.z = tracker->z.load();
	}
}
//===============================================================================

