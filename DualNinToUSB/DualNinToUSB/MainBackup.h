/*
		//sendBlank((true));
		
		//gcn64_detectController2();
		
		wdt_reset();

		if (error_count > 30)
		{
			curGamepad = NULL;
		}
		if (curGamepad == NULL)
		{
			error_count = 0;

			sendBlank(false);
			delay(30);
			int gcncont = gcn64_detectController();
			switch(gcncont)
			{
				case CONTROLLER_IS_N64:
				curGamepad = n64GetGamepad();
				curGamepad->init(false);
				error_count = 0;
				break;

				case CONTROLLER_IS_GC:
				curGamepad = gamecubeGetGamepad();
				curGamepad->init(false);
				error_count = 0;
				break;
				
				// Unknown means weird reply from the controller
				// try the old, bruteforce approach.
				case CONTROLLER_IS_UNKNOWN:
				//Check for gamecube controller
				curGamepad = gamecubeGetGamepad();
				curGamepad->init(false);
				if (curGamepad->probe(false)) {
					curGamepad = curGamepad;
					error_count = 0;
					goto detected;
				}


				delay(40);
				//Check for n64 controller
				curGamepad = n64GetGamepad();
				curGamepad->init(false);
				if (curGamepad->probe(false))  {
					curGamepad = curGamepad;
					error_count = 0;
					goto detected;
				}

				break;

			}
			goto controller2;
		}
		detected:
		wdt_reset();

		if (first_run) {
			if (curGamepad->update(false)) {
				error_count++;
				} else {
				error_count = 0;
			}
			first_run = 0;
		}
		if (!must_report)
		{
			if (curGamepad->update(false)) {
				error_count++;
				} else {
				error_count = 0;
			}
			//must_report =1;
			// Check what will have to be reported

			for (i=0; i<1; i++) {
				if (curGamepad->changed(i+1)) {
					must_report =1;
					//must_report |= (1<<i);
					
				}
			}

		}
		if(must_report)
		{
			for (i = 0; i < 1; i++)
			{
				if ((must_report & (1<<i)) == 0)
				continue;


				if (curGamepad->gamepad == 1)
				{
					sendGamecube(false);
				}
				else
				{
					sendN64(false);
				}
			}

			must_report = 0;
		}
		
		controller2:
		
		if (enable2ndController)
		{
			//Controller 2 (Only if controller is enabled
			wdt_reset();

			if (error_count2 > 30)
			{
				curGamepad2 = NULL;
			}
			if (curGamepad2 == NULL)
			{
				error_count2 = 0;

				sendBlank(true);
				delay(30);
				int gcncont = gcn64_detectController2();
				switch(gcncont)
				{
					case CONTROLLER_IS_N64:
					curGamepad2 = n64GetGamepad();
					curGamepad2->init(true);
					error_count2 = 0;
					break;

					case CONTROLLER_IS_GC:
					curGamepad2 = gamecubeGetGamepad();
					curGamepad2->init(true);
					error_count2 = 0;
					break;
					
					
					// Unknown means weird reply from the controller
					// try the old, bruteforce approach.
					case CONTROLLER_IS_UNKNOWN:
					
					// Check for gamecube controller
					curGamepad2 = gamecubeGetGamepad();
					curGamepad2->init(true);
					if (curGamepad2->probe(true)) {
						curGamepad2 = curGamepad2;
						error_count2 = 0;
						goto detected2;
					}


					delay(40);

					// Check for n64 controller
					curGamepad2 = n64GetGamepad();
					curGamepad2->init(true);
					if (curGamepad2->probe(true))  {
						curGamepad2 = curGamepad2;
						error_count2 = 0;
						goto detected2;
					}
					

					break;

				}
				continue;
			}
			detected2:
			wdt_reset();
			if (curGamepad2->update(true)) {
				error_count2++;
				} else {
				error_count2 = 0;
			}
			if (curGamepad2->gamepad == 1)
			{
				sendGamecube(true);
			}
			else
			{
				sendN64(true);
			}
			
		}
		
	*/	