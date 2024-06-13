import PyPDF2
import os

def merge_pdf_files(main_folder, subfolder, output_pdf):
    # Construct the path to the directory containing the PDFs
    pdf_dir = os.path.join(main_folder, subfolder)

    # List all PDF files in the directory, excluding mask files
    pdf_files = [f for f in os.listdir(pdf_dir) if f.endswith('.pdf') and "mask" not in f]
    pdf_files = sorted(pdf_files)  # Optional: sort files if needed

    # Create a PDF merger object
    pdf_merger = PyPDF2.PdfFileMerger()

    # Add each PDF file to the merger, excluding the mask files
    for pdf_file in pdf_files:
        full_path = os.path.join(pdf_dir, pdf_file)
        print(f"Adding {full_path} to the merger")
        pdf_merger.append(full_path)

    # Write the merged PDF to the output file within the same subdirectory
    output_path = os.path.join(pdf_dir, output_pdf)
    with open(output_path, "wb") as output_file:
        pdf_merger.write(output_file)

    # Close the merger
    pdf_merger.close()

    print(f"All PDFs merged into {output_path}")

if __name__ == "__main__":
    # Example user input
    main_folder = input("Enter the main folder name (e.g., 'tuning_20240203'): ")
    subfolder = input("Enter the subfolder name (e.g., 'C-SIDE', 'A-SIDE'): ")
    output_pdf = "merged_pdfs.pdf"  # Or take this as an input if you want

    merge_pdf_files(main_folder, subfolder, output_pdf)
