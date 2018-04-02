import json
import sys
import re

input_file = sys.argv[1]
output_file = sys.argv[2]

data = {}
with open(input_file, 'r') as content_file:
    content = content_file.read()
    data = json.loads(content)

tracks = data["tracks"]

processed_templates = set()
templates = {}
placeholders = []
new_tracks = {}

all_entries = []
for tidx in sorted([int(track) for track in tracks.keys()]):
    tidx = str(tidx)
    track = []
    for entry in tracks[tidx]:
        cell_id = entry.get("_id", None)
        entry["__track"] = tidx
        if cell_id is not None and cell_id != "":
            m = re.search(r"^<(.*)>$", cell_id)
            if m is not None:
                cell_id = m.group(1)
                entry["_id"] = cell_id
                placeholders.append(entry)
                start_time = entry["start_time"]
            else:
                print "Found template:", cell_id
                if cell_id.startswith("+"):
                    cell_id = cell_id[1:]
                    entry["_id"] = cell_id
                    all_entries.append(entry)
                entry = entry.copy()
                if not cell_id in templates:
                    templates[cell_id] = []
                templates[cell_id].append(entry)
        else:
            all_entries.append(entry)
    new_tracks[tidx] = []

for entry in placeholders:
    cell_id = entry["_id"]
    if cell_id not in processed_templates:
        processed_templates.add(cell_id)
        template = templates[cell_id]
        template.sort(key=lambda cell: cell["start_time"])
        first_start_time = template[0]["start_time"]
        first_ticks = template[0]["_ticks"]
        first_end_time = template[0]["end_time"]
        duration = first_end_time - first_start_time
        # Make all of the times relative
        for template_entry in template:
            template_entry["start_time"] -= first_start_time
            template_entry["_ticks"] -= first_ticks
            template_entry["end_time"] = duration
    # Now instantiate the template for the placeholder
    print "Instantiating template:", cell_id
    template = templates[cell_id]
    for template_entry in template:
        new_entry = template_entry.copy()
        new_entry["start_time"] += entry["start_time"]
        new_entry["_ticks"] += entry["_ticks"]
        new_entry["end_time"] += new_entry["start_time"]
        all_entries.append(new_entry)

all_entries.sort(key=lambda e: e["start_time"])

for entry in all_entries:
    track_id = entry["__track"]
    del entry["__track"]
    new_tracks[track_id].append(entry)

#data["tracks"] = {k: sorted(v, key=lambda c: c["start_time"]) for k,v in new_tracks.iteritems()}
data["tracks"] = new_tracks

with open(output_file, 'w') as content_file:
    content_file.write(json.dumps(data))

print "Done writing file."

comment = """
{
			"_id": "pyt 3",
			"blend_mode": "Normal",
			"colors": "white",
			"cycles": 5,
			"easing": "Linear",
			"inverted_easing": False,
			"leds": "G - middle",
			"name": "fade",
			"props": {},
			"_ticks": 306,
			"_duration": 1,
			"start_time": 30.6,
			"end_time": 30.700000000000003
		}

{
			"_id": "pyt 1",
			"blend_mode": "Normal",
			"colors": "dark red",
			"cycles": 1,
			"easing": "Linear",
			"inverted_easing": False,
			"leds": "J - middle",
			"name": "fade",
			"props": {},
			"_ticks": 290,
			"_duration": 15,
			"start_time": 29,
			"end_time": 30.5
		}
{
			"_id": "pyt 4",
			"blend_mode": "Normal",
			"colors": "dark red",
			"cycles": 1,
			"easing": "Linear",
			"inverted_easing": True,
			"leds": "all",
			"name": "fade",
			"props": {},
			"_ticks": 318,
			"_duration": 17,
			"start_time": 31.8,
			"end_time": 33.5
		}
{
			"_id": "pyt 2:2",
			"blend_mode": "Normal",
			"colors": "red (no fade)",
			"cycles": 2,
			"easing": "Linear",
			"inverted_easing": False,
			"leds": "all",
			"name": "swipe",
			"props": {
				"x": 1,
				"y": 1
			},
			"_ticks": 305,
			"_duration": 10,
			"start_time": 30.5,
			"end_time": 31.5
		}
{
			"_id": "pyt 5:1",
			"blend_mode": "Normal",
			"colors": "blue (no fade)",
			"cycles": 1,
			"easing": "Linear",
			"inverted_easing": False,
			"leds": "J",
			"name": "swipe",
			"props": {
				"x": 1,
				"y": 1
			},
			"_ticks": 320,
			"_duration": 10,
			"start_time": 32,
			"end_time": 33
		}


{
			"_id": "pyt 2:3",
			"blend_mode": "Normal",
			"colors": "orange (no fade)",
			"cycles": 2,
			"easing": "Linear",
			"inverted_easing": False,
			"leds": "all",
			"name": "swipe",
			"props": {
				"x": -1,
				"y": -1
			},
			"_ticks": 305,
			"_duration": 10,
			"start_time": 30.5,
			"end_time": 31.5
		}
{
			"_id": "pyt 5:2",
			"blend_mode": "Normal",
			"colors": "purple (no fade)",
			"cycles": 1,
			"easing": "Linear",
			"inverted_easing": False,
			"leds": "G",
			"name": "swipe",
			"props": {
				"x": -1,
				"y": -1
			},
			"_ticks": 320,
			"_duration": 10,
			"start_time": 32,
			"end_time": 33
		}
"""
