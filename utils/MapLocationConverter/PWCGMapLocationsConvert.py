import json
import xml.etree.ElementTree as ET

# Load JSON data
with open('MapLocations.json', 'r', encoding='utf-8') as f:
    data = json.load(f)

# Create root element
root = ET.Element('Locations')

# Iterate over location entries
for loc in data.get('locations', []):
    name = loc.get('name', '').strip()

    # Create Location element
    location_elem = ET.SubElement(root, 'Location')
    location_elem.set('id', name)

    # Extract position
    pos = loc.get('position', {})
    x = str(pos.get('xPos', 0.0))
    y = str(pos.get('yPos', 0.0))
    z = str(pos.get('zPos', 0.0))

    # Create Position sub-element
    pos_elem = ET.SubElement(location_elem, 'Position')
    pos_elem.set('x', x)
    pos_elem.set('y', y)
    pos_elem.set('z', z)

# Write to XML file
tree = ET.ElementTree(root)
tree.write('ConvertedLocations.xml', encoding='utf-8', xml_declaration=True)