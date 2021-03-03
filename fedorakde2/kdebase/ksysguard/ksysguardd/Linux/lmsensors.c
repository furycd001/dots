/*
    KSysGuard, the KDE System Guard

    Copyright (c) 1999 - 2001 Chris Schlaeger <cs@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

//#define HAVE_SENSORS_SENSORS_H

//#include <config.h>

#ifdef HAVE_SENSORS_SENSORS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sensors/sensors.h>

#include <ccont.h>

#include "Command.h"
#include "ksysguardd.h"
#include "lmsensors.h"


#ifndef SENSORS_API_VERSION
#define SENSORS_API_VERSION 0x000
#endif
#ifndef SENSORS_CHIP_NAME_BUS_PCI
#define SENSORS_CHIP_NAME_BUS_PCI -5
#endif
#ifndef SENSORS_CHIP_NAME_BUS_ISA
#define SENSORS_CHIP_NAME_BUS_ISA -1
#endif

#define BUFFER_SIZE_LMSEN 300
typedef struct
{
	char* fullName;
	const sensors_chip_name* scn;
	const sensors_feature *sf;
	const sensors_subfeature *sfd;
} LMSENSOR;

static CONTAINER LmSensors;
static int LmSensorsOk = -1;

static int 
sensorCmp(void* s1, void* s2)
{
	return (strcmp(((LMSENSOR*) s1)->fullName,
				   ((LMSENSOR*) s2)->fullName));
}

static LMSENSOR*
findMatchingSensor(const char* name)
{
	INDEX index;
	LMSENSOR key;
	LMSENSOR* s;

	if (name == NULL || name[0] == '\0') return 0;
	key.fullName = strdup(name);
	int end = strlen(key.fullName)-1;
	if (key.fullName[end] == '?')
		key.fullName[end] = '\0';
	if ((index = search_ctnr(LmSensors, sensorCmp, &key)) < 0)
	{
		free(key.fullName);
		return (0);
	}

	free(key.fullName);
	s = get_ctnr(LmSensors, index);

	return (s);
}

static const char *chipName(const sensors_chip_name *chip)
{
	static char buffer[256];
	sensors_snprintf_chip_name(buffer, sizeof(buffer), chip);
	return buffer;
}

void
initLmSensors(void)
{
	const sensors_chip_name* scn;
	int nr = 0;

	FILE* input;
	input = fopen("/etc/sensors.conf", "r");

	if (sensors_init(input))
	{
		LmSensorsOk = -1;
		if (input)
			fclose(input);
		return;
	}
	if (input)
		fclose(input);

	LmSensors = new_ctnr(CT_SLL);
	while ((scn = sensors_get_detected_chips(NULL, &nr)) != NULL)
	{
		int nr1 = 0;
		const sensors_feature* sf;

		while ((sf = sensors_get_features(scn, &nr1)) != 0) {
			const sensors_subfeature *ssubf;
			LMSENSOR *p;
			char *s;
			char *label;
			char scnbuf[BUFFER_SIZE_LMSEN];

			switch (sf->type)
			{
			case SENSORS_FEATURE_IN:
				ssubf = sensors_get_subfeature(scn, sf,
				        SENSORS_SUBFEATURE_IN_INPUT);
				break;

			case SENSORS_FEATURE_FAN:
				ssubf = sensors_get_subfeature(scn, sf,
				        SENSORS_SUBFEATURE_FAN_INPUT);
				break;

			case SENSORS_FEATURE_TEMP:
				ssubf = sensors_get_subfeature(scn, sf,
				        SENSORS_SUBFEATURE_TEMP_INPUT);
				break;
			default:
				ssubf = NULL;
			}

			if (!ssubf)
				continue;

			label = sensors_get_label(scn, sf);
			sensors_snprintf_chip_name(scnbuf, BUFFER_SIZE_LMSEN, scn);
			p = (LMSENSOR*)malloc(sizeof(LMSENSOR));
			p->fullName = (char*)malloc(strlen("lmsensors/") +
			        strlen(scnbuf) + 1 +
			        strlen(label) + 1);
			snprintf(p->fullName, BUFFER_SIZE_LMSEN, "lmsensors/%s/%s", scnbuf, label);

			/* Make sure that name contains only proper characters. */
			for (s = p->fullName; *s; s++)
				if (*s == ' ')
					*s = '_';

			p->scn = scn;
			p->sf = sf;
			p->sfd = ssubf;

			/* Note a name collision should never happen with the lm_sensors-3x code,
			   but it does in the case of k8temp, when there are 2 identical labeled
			   sensors per CPU. This are really 2 distinct sensors measuring the
			   same thing, but fullName must be unique so we just drop the second
			   sensor */
			if (search_ctnr(LmSensors, sensorCmp, p) < 0) {
				push_ctnr(LmSensors, p);
				registerMonitor(p->fullName, "float", printLmSensor,
				    printLmSensorInfo);
			}
			else
			{
				free(p->fullName);
				free(p);
			}
			free(label);
		}
	}
	bsort_ctnr(LmSensors, sensorCmp, 0);
}

void
exitLmSensors(void)
{
	destr_ctnr(LmSensors, free);
}

void
printLmSensor(const char* cmd)
{
	double value;
	LMSENSOR* s;

	if ((s = findMatchingSensor(cmd)) == 0)
	{
		/* should never happen */
		fprintf(CurrentClient, "0\n");
		return;
	}
	sensors_get_value(s->scn, s->sfd->number, &value);
	fprintf(CurrentClient, "%f\n", value);
}

void
printLmSensorInfo(const char* cmd)
{
	LMSENSOR* s;

	if ((s = findMatchingSensor(cmd)) == 0)
	{
		/* should never happen */
		fprintf(CurrentClient, "0\n");
		return;
	}

	/* TODO: print real name here */
	char *label;
	label = sensors_get_label(s->scn, s->sf);
	if (label == NULL)
	{
		fprintf(CurrentClient, "0\n");
		return;
	}
	if (strncmp(s->sfd->name, "temp", sizeof("temp")-1) == 0)
		fprintf(CurrentClient, "%s\t0\t0\t°C\n", label);
	else if (strncmp(s->sfd->name, "fan", sizeof("fan")-1) == 0)
		fprintf(CurrentClient, "%s\t0\t0\trpm\n", label);
	else
		fprintf(CurrentClient, "%s\t0\t0\tV\n", label);  /* For everything else, say it's in volts. */
	free(label);
}

#else /* HAVE_SENSORS_SENSORS_H */

/* dummy version for systems that have no lmsensors support */

void
initLmSensors(void)
{
}

void
exitLmSensors(void)
{
}

#endif
