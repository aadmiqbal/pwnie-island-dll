void spawn_actor_with_id(unsigned int id, float x, float y, float z) {
	log("spawn_actor_with_id: Function start");

	uintptr_t fn_spawn_actor_with_id = (uintptr_t)moduleBase + 0x630C0;
	uintptr_t vec3_addr = (uintptr_t)moduleBase + 0x14A0;
	uintptr_t bearfunction = (uintptr_t)moduleBase + 0x6010;
	uintptr_t world_instance = (uintptr_t)moduleBase + 0x00097d7c;
	char* world = (char*)world_instance;

	log("spawn_actor_with_id: Addresses assigned");

	void* pos = std::malloc(500); // Attempt to allocate memory
	void* actor = std::malloc(10000); // Attempt to allocate memory

	// Check if memory allocation was successful
	if (pos == NULL || actor == NULL) {
		log("spawn_actor_with_id: Memory allocation failed");
		// Optionally, free any successfully allocated memory before returning
		if (pos != NULL) {
			std::free(pos);
		}
		if (actor != NULL) {
			std::free(actor);
		}
		return;
	}

		log("spawn_actor_with_id: Memory allocated for pos and actor");

		// Prepare Vector3 object
		log("spawn_actor_with_id: Preparing Vector3 object");
		__asm {
			lea ecx, [pos]
			movss xmm1, x
			movss xmm2, y
			movss xmm3, z
			call vec3_addr
		}
		log("spawn_actor_with_id: Vector3 object prepared");

		// Prepare Bear object
		log("spawn_actor_with_id: Preparing Bear object");
		__asm {
			lea ecx, [actor]
			call bearfunction
		}
		log("spawn_actor_with_id: Bear object prepared");

		// Call spawn actor
		log("spawn_actor_with_id: Calling fn_spawn_actor_with_id");
		__asm {
			push pos // Using pos as a placeholder for Rotation
			push pos // Pointer to Vector3 (position)
			push actor // Pointer to Bear (actor)
			push id // Actor ID
			mov ecx, world // 'this' pointer for World object
			call fn_spawn_actor_with_id
		}
		log("spawn_actor_with_id: Function executed");

		std::free(pos);
		std::free(actor);

		log("spawn_actor_with_id: Memory freed, function end");
	}
