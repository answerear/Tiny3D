import argparse
import pandas as pd
import logging


def main(input_file, output_h, output_txt):
    logging.info('Reading input Excel file: {}'.format(input_file))
    df = pd.read_excel(input_file)
    logging.info('Input Excel file has {} rows.'.format(len(df)))

    logging.info('Writing output C++ header file: {}'.format(output_h))
    logging.info('Writing output text file: {}'.format(output_txt))
    with open(output_h, 'w') as h_file, open(output_txt, 'w', encoding='utf-8') as txt_file:
        h_file.write("// Generated code exported from excel_to_hpp.\n")
        h_file.write("// DO NOT modify this manually! Edit the corresponding language excel file instead!\n\n")
        h_file.write("#pragma once\n\n")
        for index, row in df.iterrows():
            macro_name = row[0]
            macro_value = index
            h_file.write('#define {} {}\n'.format(macro_name, macro_value))
            #logging.info('Writing macro definition: #define {} {}'.format(macro_name, macro_value))

            text = row[1]
            txt_file.write('{}\n'.format(text))
            #logging.info('Writing text: {}'.format(text))

    logging.info('Conversion completed successfully.')

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

    parser = argparse.ArgumentParser(description='Convert Excel file to C++ header and text file.')
    parser.add_argument('input_file', type=str, help='Input Excel file.')
    parser.add_argument('output_h', type=str, help='Output C++ header file.')
    parser.add_argument('output_txt', type=str, help='Output text file.')

    args = parser.parse_args()

    main(args.input_file, args.output_h, args.output_txt)