// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <math.h>

#define M_PI 3.14159265358979323846

struct Vector3
{
	float x, y, z;
};

struct Angle
{
	float yaw, pitch, roll;
};

class Player
{
public:
	char pad_0000[4]; //0x0000
	Vector3 playerCoords; //0x0004
	char pad_0010[48]; //0x0010
	Angle angle; //0x0040
	char pad_004C[172]; //0x004C
	uint32_t health; //0x00F8
	uint32_t armor; //0x00FC
	char pad_0100[20]; //0x0100
	uint32_t pistol; //0x0114
	uint32_t carbine; //0x0118
	uint32_t shotgun; //0x011C
	uint32_t machingun; //0x0120
	uint32_t sniper; //0x0124
	uint32_t ar; //0x0128
	char pad_012C[16]; //0x012C
	uint32_t pistolClip; //0x013C
	uint32_t carbineClip; //0x0140
	uint32_t shotgunClip; //0x0144
	uint32_t machinegunClip; //0x0148
	uint32_t sniperClip; //0x014C
	uint32_t arClip; //0x0150
	char pad_0154[4]; //0x0154
	uint32_t grenade; //0x0158
	char pad_015C[4]; //0x015C
	uint32_t knifeTime; //0x0160
	uint32_t pistolTime; //0x0164
	uint32_t carbineTime; //0x0168
	uint32_t shotgunTime; //0x016C
	uint32_t machinegunTime; //0x0170
	uint32_t sniperTime; //0x0174
	uint32_t arTime; //0x0178
	char pad_017C[4]; //0x017C
	uint32_t grenadeTime; //0x0180
	char pad_0184[128]; //0x0184
	uint32_t teamNum1; //0x0204
	char pad_0208[304]; //0x0208
	uint32_t isAlive; //0x0338
	char pad_033C[1296]; //0x033C
}; //Size: 0x084C



float euclidean_dist(float x, float y)
{
	return sqrtf((x * x) + (y * y));
}

DWORD WINAPI aimbot_thread(HMODULE hModule)
{
	//aimbot_thread(hModule);
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Injected Succesfully!\nPRESS NUMPAD 0 to Unload DLL.\n" << std::endl;

    while (true)
    {
		uintptr_t* player_ptr = (uintptr_t*)0x509B74;
		Player* player = (Player*)(*player_ptr);
		uintptr_t* entityList = (uintptr_t*)0x50F4F8;
		int* curr_players = (int*)0x50F500;

		float closest_player = -1.0f;
		float closest_yaw = 0.0f;
		float closest_pitch = 0.0f;

		for (int i = 0; i < *curr_players; i++)
		{
			uintptr_t* enemy_offset = (uintptr_t*)(*entityList + (i * 4));
			Player* enemy = (Player*)(*enemy_offset);

			if (player != NULL && enemy != NULL && !enemy->isAlive)
			{
				float abspos_x = enemy->playerCoords.x - player->playerCoords.x;
				float abspos_y = enemy->playerCoords.y - player->playerCoords.y;
				float abspos_z = enemy->playerCoords.z - player->playerCoords.z;

				float temp_dist = euclidean_dist(abspos_x, abspos_y);

				if (closest_player == -1.0f || temp_dist < closest_player)
				{
					closest_player = temp_dist;

					float azimuth_xy = atan2f(abspos_y, abspos_x);
					closest_yaw = (float)(azimuth_xy * (180.0 / M_PI));
					
					if (abspos_y < 0)
					{
						abspos_y *= -1;
					}
					if (abspos_y < 5)
					{
						if (abspos_x < 0)
						{
							abspos_x *= -1;
						}
						abspos_y = abspos_x;
					}

					float azimuth_yz = atan2f(abspos_z, abspos_y);
					closest_pitch = (float)(azimuth_yz * (180.0 / M_PI));
				}


			}

		}

		player->angle.yaw = closest_yaw + 90;
		player->angle.pitch = closest_pitch;

		

		if (GetAsyncKeyState(VK_NUMPAD0))
		{
			
			std::cout << "Unloaded!\n" << std::endl;
			Sleep(3000);
			break;
		}

        Sleep(10);
    }

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);

	return 0;
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)aimbot_thread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

