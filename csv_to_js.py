import pandas as pd
import json

def convert_to_js():
    df = pd.read_csv("gravity_table.csv")
    
    # Round to 5 decimal places to save space
    df = df.round(5)
    
    # Convert to list of lists
    data_list = df.values.tolist()
    
    # Create JS content
    js_content = f"export const GRAVITY_TABLE = {json.dumps(data_list)};\n"
    js_content += f"export const GRAVITY_COLUMNS = {json.dumps(list(df.columns))};\n"
    
    with open("plugins/gravity-well/src/gravity_data.js", "w") as f:
        f.write(js_content)
        
    print("Converted gravity_table.csv to src/gravity_data.js")

if __name__ == "__main__":
    convert_to_js()
