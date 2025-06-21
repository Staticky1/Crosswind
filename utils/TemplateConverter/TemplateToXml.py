import xml.etree.ElementTree as ET

def preprocess_lines(lines):
    """Merge lines like `Block` + `{` into `Block {` for easier parsing."""
    processed = []
    skip_next = False
    for i, line in enumerate(lines):
        if skip_next:
            skip_next = False
            continue
        stripped = line.strip()
        if i + 1 < len(lines) and lines[i + 1].strip() == "{":
            processed.append(f"{stripped} {{")
            skip_next = True
        else:
            processed.append(stripped)
    return processed

def parse_block(lines, index):
    """Parse a single object block (recursive)."""
    root = None
    stack = []

    while index < len(lines):
        line = lines[index].strip()
        index += 1

        if not line:
            continue

        if line.endswith("{"):
            tag = line[:-1].strip()
            elem = ET.Element(tag)
            if stack:
                stack[-1].append(elem)
            else:
                root = elem
            stack.append(elem)

        elif line == "}":
            stack.pop()
            if not stack:
                break  # Block finished

        elif "=" in line:
            parts = line.rstrip(";").split("=", 1)
            key = parts[0].strip()
            value = parts[1].strip().strip('"').rstrip(";")
            if stack:
                if key.isdigit():
                    stack[-1].set(f"key_{key}", value)
                else:
                    stack[-1].set(key, value)

        else:
            print(f"Unrecognized line: {line}")

    return root, index

def parse_file_to_xml(file_path):
    with open(file_path, 'r') as file:
        raw_lines = file.readlines()

    lines = preprocess_lines(raw_lines)
    index = 0
    root = ET.Element("Mission")  # Wrap all top-level objects

    while index < len(lines):
        element, index = parse_block(lines, index)
        if element is not None:
            root.append(element)

    return ET.ElementTree(root)

def save_xml_tree(tree, output_path):
    tree.write(output_path, encoding='utf-8', xml_declaration=True)

def main():
    input_file = "template.txt"
    output_file = "output.xml"

    tree = parse_file_to_xml(input_file)
    save_xml_tree(tree, output_file)
    print(f"Parsed all mission objects to: {output_file}")

if __name__ == "__main__":
    main()