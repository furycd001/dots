/*
    KSysGuard, the KDE System Guard
	   
    Copyright (c) 2001 Tobias Koenig <tokoe82@yahoo.de>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Command.h"
#include "conf.h"

CONTAINER LogFileList = 0;
CONTAINER SensorList = 0;

void parseConfigFile(const char *filename)
{
	FILE* config;
	char line[2048];
	char *begin, *token, *tmp, *confSensor;
	ConfigLogFile *confLog;
	

	if ((config = fopen(filename, "r")) == NULL) {
		log_error("can't open config file '%s'", filename);
		return;
	}

	
	if (LogFileList)
		destr_ctnr(LogFileList, free);

	if (SensorList)
		destr_ctnr(SensorList, free);

	LogFileList = new_ctnr(CT_DLL);
	SensorList = new_ctnr(CT_DLL);

	while (fgets(line, sizeof(line), config) != NULL) {
		if ((line[0] == '#') || (strlen(line) == 0)) {
			continue;
		}

		if (strchr(line, '#'))
			*(strchr(line, '#')) = '\0';

		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';

		if (!strncmp(line, "LogFiles", 8)) {
			begin = strchr(line, '=');
			begin++;

			for (token = strtok(begin, ","); token; token = strtok(NULL, ",")) {
				if ((confLog = (ConfigLogFile *)malloc(sizeof(ConfigLogFile))) == NULL) {
					log_error("malloc() no free memory avail");
					continue;
				}
				confLog->name = strdup(token);
				tmp = strchr(confLog->name, ':');
				*tmp = '\0';
				confLog->path = tmp;
				confLog->path++;

				push_ctnr(LogFileList, confLog);
			}
		}
		if (!strncmp(line, "Sensors", 7)) {
			begin = strchr(line, '=');
			begin++;

			for (token = strtok(begin, ","); token; token = strtok(NULL, ",")) {
				confSensor = strdup(token);
				push_ctnr(SensorList, confSensor);
			}
		}
	}

	fclose(config);
}

int sensorAvailable(const char *sensor)
{
	int i;
	
	for (i = 0; i < level_ctnr(SensorList); i++) {
		char* name = get_ctnr(SensorList, i);
		if (!strcmp(name, sensor))
			return 1;
	}

	return 0;
}
