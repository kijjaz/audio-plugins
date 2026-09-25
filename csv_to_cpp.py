import pandas as pd

def convert_to_cpp_header():
    df = pd.read_csv("gravity_table.csv")
    df = df.round(5)
    
    # 10000 rows, 12 columns
    rows = df.shape[0]
    cols = df.shape[1]
    
    # Create flattened array content
    # float gravityData[] = { ... };
    
    # We'll write it flat: row0_col0, row0_col1, ...
    flat_data = df.values.flatten()
    
    header_content = "#pragma once\n\n"
    header_content += f"const int kGravityRows = {rows};\n"
    header_content += f"const int kGravityCols = {cols};\n\n"
    header_content += "const float kGravityData[] = {\n"
    
    # Write in chunks to avoid massive memory usage or slow string concat
    chunk_str = ""
    for i, val in enumerate(flat_data):
        chunk_str += f"{val}f,"
        if (i + 1) % 12 == 0:
            chunk_str += "\n"
        else:
             chunk_str += " "
    
    # Remove last comma
    header_content += chunk_str.rstrip(",\n")
    header_content += "\n};\n"
    
    # Also define column indices for easy access
    col_names = list(df.columns)
    header_content += "\n// Column Indices\n"
    for i, name in enumerate(col_names):
        header_content += f"const int kIdx_{name} = {i};\n"
        
    with open("vst3/GravityWell/Source/GravityData.h", "w") as f:
        f.write(header_content)
        
    print(f"Converted gravity_table.csv to vst3/GravityWell/Source/GravityData.h ({len(flat_data)} floats)")

if __name__ == "__main__":
    convert_to_cpp_header()
