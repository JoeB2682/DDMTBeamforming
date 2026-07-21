//===============================================================================
#include "MotionTrackerHandler.h"
//===============================================================================
// Constructor and Destructor
MotionTrackerHandler::MotionTrackerHandler() 
{
	// Client connects to NatNet server and handles motiv data
	client = new NatNetClient();

	client->SetFrameReceivedCallback(DataHandler, this);

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
		client->Disconnect();

		delete client;
		client = nullptr;
	}
}
//===============================================================================
// Connect to Motive on local serevr
void MotionTrackerHandler::connect()
{
	sNatNetClientConnectParams params;

	params.connectionType = ConnectionType_Unicast;

	// Local Interface IP in Motive
	//params.serverAddress = "192.168.56.1";
	//params.localAddress = "192.168.56.1";

	params.serverAddress = "localhost";
	params.localAddress = "localhost";

	//params.serverAddress = "127.0.0.1";
	//params.localAddress = "127.0.0.1";

	params.serverCommandPort = 1510;
	params.serverDataPort = 1511;

	client->Connect(params);

	int result = client->Connect(params);
	DBG("Connect result: " << result);
}
//===============================================================================
// Get motion tracking data and store
void NATNET_CALLCONV MotionTrackerHandler::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	// Create pointer to current object
	auto* tracker = static_cast<MotionTrackerHandler*>(pUserData);

	// Loop through rigidbody data
	if (data->nRigidBodies > 0)
	{
		// Get data point
		auto& rb = data->RigidBodies[0];

		DBG("RB: " << rb.x << " " << rb.y << " " << rb.z);

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

