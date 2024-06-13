import ROOT
from array import array
import xml.etree.ElementTree as ET
import datetime
import os  
import matplotlib.pyplot as plt
import numpy as np
import re
import glob

# Assuming AtlasStyle.C has been loaded and applied previously
ROOT.gROOT.LoadMacro('AtlasStyle.C')
ROOT.SetAtlasStyle()

ROOT.gROOT.SetBatch(True)  # Ensure ROOT doesn't try to display canvases interactively

def create_filled_graph(fit_function, xmin, xmax, color):
    """Create a TGraph for both line and filled area with adjusted transparency."""
    n_points = 10000
    x = array('d', [xmin + i * (xmax - xmin) / n_points for i in range(n_points + 1)])
    y = array('d', [fit_function.Eval(xi) for xi in x])
    
    graph = ROOT.TGraph(len(x), x, y)
    graph.SetFillColorAlpha(color, 0.5)
    graph.SetFillStyle(1006)
    graph.SetLineColor(color)
    graph.SetLineWidth(1)
    graph.SetLineStyle(2)
    return graph

def plot_threshold_distribution(xml_file, output_pdf_filename,location_name):
    ROOT.gStyle.SetOptStat(0)
    # Explicitly enable titles, in case ATLAS style disables them
    ROOT.gStyle.SetOptTitle(1)
    
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    canvas = ROOT.TCanvas("canvas", "Threshold Distribution", 1200, 600)
    # Adjust the top margin to create more space between the title and the plot
    canvas.SetTopMargin(0.1)  # Increase top margin
    
    mg = ROOT.TMultiGraph()

    # Move the legend to the top-right corner and adjust its size
    legend = ROOT.TLegend(0.62, 0.62, 0.95, 0.90)
    legend.SetTextSize(0.03)  # Adjust text size if needed
    
    color_index = 6
    
    for detector in root.findall('detector'):
        chipSN = detector.get('chipSN')
        path = detector.get('path')
        
        for threshold in detector.findall('threshold'):
            run = threshold.get('run')
            
            # Plot Threshold Distribution
            file_path = f"{path}THRESHOLD_SCAN_{run}/analysis.root"
            histo_name = f"threshdist_Mod_{chipSN};1"
            
            file = ROOT.TFile.Open(file_path)
            if not file or file.IsZombie():
                continue
            
            histo = file.Get(histo_name)
            if not histo:
                continue
            
            fit_function = ROOT.TF1("fit_func", "gaus", 1800, 2200)
            histo.Fit(fit_function, "SQ")
            
            graph = create_filled_graph(fit_function, 1800, 2200, color_index)
            mg.Add(graph, "F")
            
            mu = fit_function.GetParameter(1)
            sigma = fit_function.GetParameter(2)
            legend.AddEntry(graph, f"P{chipSN}: #mu={mu:.2f}, #sigma={sigma:.2f}", "f")
            
            color_index += 1
    
    # Ensure the title is properly set and displayed
    mg.SetTitle(f"Threshold Distribution for {location_name} ;Value;Counts")
    mg.Draw("A")
    legend.Draw()
    
    canvas.Modified()
    canvas.Update()

    # Save the PDF with the provided filename
    canvas.SaveAs(output_pdf_filename)

def plot_tot_distribution(xml_file, output_pdf_filename,location_name):
    ROOT.gStyle.SetOptStat(0)
    # Explicitly enable titles, in case ATLAS style disables them
    ROOT.gStyle.SetOptTitle(1)
    
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    canvas = ROOT.TCanvas("canvas", "TOT Distribution", 1200, 600)
    # Adjust the top margin to create more space between the title and the plot
    canvas.SetTopMargin(0.1)  # Increase top margin
    
    mg = ROOT.TMultiGraph()

    # Move the legend to the top-right corner and adjust its size
    legend = ROOT.TLegend(0.62, 0.62, 0.95, 0.90)
    legend.SetTextSize(0.03)  # Adjust text size if needed
    
    color_index = 6
    
    for detector in root.findall('detector'):
        chipSN = detector.get('chipSN')
        path = detector.get('path')
        
        for tot in detector.findall('tot'):
            run = tot.get('run')
            
            # Plot TOT Distribution
            file_path = f"{path}TOT_TEST_{run}/analysis.root"
            histo_name = f"ToT_Mean_Dist_Mod_{chipSN};1"
            
            file = ROOT.TFile.Open(file_path)
            if not file or file.IsZombie():
                continue
            
            histo = file.Get(histo_name)
            if not histo:
                continue
            
            fit_function = ROOT.TF1("fit_func", "gaus", 6, 14)
            histo.Fit(fit_function, "SQ")
            
            graph = create_filled_graph(fit_function, 6, 14, color_index)
            mg.Add(graph, "F")
            
            mu = fit_function.GetParameter(1)
            sigma = fit_function.GetParameter(2)
            legend.AddEntry(graph, f"P{chipSN}: #mu={mu:.2f}, #sigma={sigma:.2f}", "f")
            
            color_index += 1
    
    # Ensure the title is properly set and displayed
    mg.SetTitle(f"TOT Distribution for {location_name};Value;Counts")
    mg.Draw("A")
    legend.Draw()
    
    canvas.Modified()
    canvas.Update()

    # Save the PDF with the provided filename
    canvas.SaveAs(output_pdf_filename)

def plot_sigma_distribution(xml_file, output_pdf_filename,location_name):
    ROOT.gStyle.SetOptStat(0)
    # Explicitly enable titles, in case ATLAS style disables them
    ROOT.gStyle.SetOptTitle(1)
    
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    canvas = ROOT.TCanvas("canvas", "Sigma Distribution", 1200, 600)
    # Adjust the top margin to create more space between the title and the plot
    canvas.SetTopMargin(0.1)  # Increase top margin
    
    mg = ROOT.TMultiGraph()

    # Move the legend to the top-right corner and adjust its size
    legend = ROOT.TLegend(0.62, 0.62, 0.95, 0.90)
    legend.SetTextSize(0.03)  # Adjust text size if needed
    
    color_index = 6
    
    for detector in root.findall('detector'):
        chipSN = detector.get('chipSN')
        path = detector.get('path')
        
        for threshold in detector.findall('threshold'):
            run = threshold.get('run')
            
            # Plot Sigma Distribution
            file_path = f"{path}THRESHOLD_SCAN_{run}/analysis.root"
            histo_name = f"sigmadist_Mod_{chipSN};1"
            
            file = ROOT.TFile.Open(file_path)
            if not file or file.IsZombie():
                continue
            
            histo = file.Get(histo_name)
            if not histo:
                continue
            
            fit_function = ROOT.TF1("fit_func", "gaus", 50,300)
            histo.Fit(fit_function, "SQ")
            
            graph = create_filled_graph(fit_function, 50, 300, color_index)
            mg.Add(graph, "F")
            
            mu = fit_function.GetParameter(1)
            sigma = fit_function.GetParameter(2)
            legend.AddEntry(graph, f"P{chipSN}: #mu={mu:.2f}, #sigma={sigma:.2f}", "f")
            
            color_index += 1
    
    # Ensure the title is properly set and displayed
    mg.SetTitle(f"Noise Distribution for {location_name};Value;Counts")
    mg.Draw("A")
    legend.Draw()
    
    canvas.Modified()
    canvas.Update()

    # Save the PDF with the provided filename
    canvas.SaveAs(output_pdf_filename)
def parse_mask_file(mask_file_path):
    """
    Parse the mask file to create a 2D array of mask data.
    """
    with open(mask_file_path, 'r') as file:
        lines = file.readlines()[1:]  # Skip the first line
        
    mask_data = []
    for line in lines:
        parts = line.strip().split()[1:]  # Split by space and remove the first column
        line_data = ''.join(parts)  # Join to form a single string
        line_data = line_data.replace('-', '')  # Remove dashes
        mask_row = [int(char) for char in line_data]  # Convert string to list of integers (0s and 1s)
        mask_data.append(mask_row)
    
    return np.array(mask_data)

import matplotlib.colors as mcolors
def plot_mask(mask_data, output_file, title):
    """
    Plot the mask data with a larger square overall plot shape, thicker grid lines for better visibility,
    and without row numbering for clarity.
    """
    nrows, ncols = mask_data.shape
    
    # Increase figure size for better visibility, especially on full screen
    fig_size = 16  # Adjust this value as needed for a larger plot
    fig, ax = plt.subplots(figsize=(fig_size, fig_size))  # Create a larger square figure
    
    # Custom colormap: 0s will be white, 1s will be red
    cmap = mcolors.ListedColormap(['white', 'red'])
    bounds = [-0.5, 0.5, 1.5]
    norm = mcolors.BoundaryNorm(bounds, cmap.N)
    
    # Plot the mask data with 'auto' aspect to optimally utilize the plot area
    cax = ax.matshow(mask_data, cmap=cmap, norm=norm, aspect='auto') # Thicker grid lines for enhanced visibility
    ax.set_xticks(np.arange(-0.5, ncols, 1), minor=True)
    ax.set_yticks(np.arange(-0.5, nrows, 1), minor=True)
    ax.grid(which="minor", color="k", linestyle='-', linewidth=1)  # Increase linewidth here
    
    # Set the major ticks to label the pixels, adjusting for visibility
    ax.set_xticks(np.arange(0, ncols, 1))
    # Removing y-tick labels as requested
    ax.set_xticklabels(np.arange(1, ncols + 1))
    
    # Correcting the tick label visibility issue
    ax.tick_params(axis='x', which="both", bottom=True, top=False, labelbottom=True)
    ax.tick_params(axis='y', which="both", left=True, right=False, labelleft=False)  # Disable y labels
    
    # Rotate x-tick labels to prevent overlapping and adjust font size for clarity
    plt.xticks(rotation=90, fontsize=4)
    
    zero_pixels = np.sum(mask_data == 0)
    plt.title(f"{title}, Masked pixels: {zero_pixels}", size=12)
    
    plt.savefig(output_file, format='pdf', dpi=300)  # Save as PDF with high resolution
    plt.close()

def plot_mask_distribution(xml_file, output_pdf_directory,location_name):
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    for detector in root.findall('detector'):
        chipSN = detector.get('chipSN')
        path = detector.get('path')
        
        for noise in detector.findall('source'):
            run = noise.get('run')
            run1 = noise.get('config')  # Assuming 'run1' attribute is available here
            module = noise.get('module')
            
            # Construct the file pattern using run1
            file_pattern = os.path.join(path, f"NOISESCAN_SELFTRIGGER_{run}/config/{module}/masks/enable_{module}__{run1}.dat")
            
            # Use glob to find matching files
            mask_files = glob.glob(file_pattern)
            if not mask_files:
                print(f"No mask files found for pattern: {file_pattern}")
                continue
            
            for mask_file in mask_files:
                print(f"Processing mask file: {mask_file}")
                mask_data = parse_mask_file(mask_file)
                output_file = os.path.join(output_pdf_directory, f"mask_for_P{chipSN}_{location_name}.pdf")
                plot_mask(mask_data, output_file,f"{module} at P{chipSN} for {location_name}")

if __name__ == "__main__":
    location_name = input("Enter the location name (e.g., A-NEAR, A-FAR, C-NEAR, C-FAR): ")
    today = datetime.date.today().strftime("%Y%m%d")
    folder_name = f"tuning_{today}/{location_name}"
    if not os.path.exists(folder_name):
        os.makedirs(folder_name)

    xml_file = "config.xml"
    
    # The following lines call the plot functions for threshold, tot, sigma, and now masks
    # Assuming those functions are defined above or elsewhere in your script
    threshold_pdf = f"{folder_name}/threshold_distribution_{location_name}_{today}.pdf"
    tot_pdf = f"{folder_name}/tot_distribution_{location_name}_{today}.pdf"
    sigma_pdf = f"{folder_name}/noise_distribution_{location_name}_{today}.pdf"
    mask_pdf = f"{folder_name}/" 
    # Example calls (uncomment and adjust according to your actual function implementations)
    plot_threshold_distribution(xml_file, threshold_pdf,location_name)
    plot_tot_distribution(xml_file, tot_pdf,location_name)
    plot_sigma_distribution(xml_file, sigma_pdf,location_name)
    plot_mask_distribution(xml_file, mask_pdf,location_name)
