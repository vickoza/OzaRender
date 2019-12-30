#include "instance.h"
instance::instance(const char* masterName) : shape("instance")
{
	master = nullptr;
	SetMaster(masterName);
}

void instance::DrawShape(DrawingObject& dobj)
{
	if (master != nullptr)
	{
		//glPushMatrix();
		master->Draw();
		//glPopMatrix();
	}
}

void instance::SetMaster(const char* masterName)
{
	/*char tmpName[MAX_NAME_LENGTH];//HACK

	if ((masterName != NULL) && (masterName[0] != '\0'))
	{
		int i = -1; // So that the list will start at -1 + 1 = 0
		do
		{
			i++;
			sprintf(tmpName, "%s_%03d", masterName, i);
		} while (FindShape(tmpName) != NULL);
		master = shape::FindShape(masterName);
		if (master == nullptr) /
			complain("Cannot find master object %s", masterName);
	}
	else
		sprintf(tmpName, "instance");
	SetObjectName(tmpName);*/
}

void instance::ReadObject(Loader& input)
{
	char tmpName[MAX_NAME_LENGTH];

	input.ReadToken(tmpName);

	if (!shape::FindShape(tmpName))
		input.Error(tmpName, "Couldn't find shape %s for instance in component", tmpName);

	SetMaster(tmpName);  // This set's the object name to "masterName"_InstanceCount

	input.EatNumbers();  // Sometimes the master's object count follows ... ignore it
	ReadAttributes(input);
}