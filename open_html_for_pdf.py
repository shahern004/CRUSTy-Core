import os
import webbrowser
import platform
import subprocess

def open_html_file(html_file):
    """
    Opens an HTML file in the default browser and provides instructions for saving as PDF.
    """
    # Get the absolute path to the HTML file
    abs_path = os.path.abspath(html_file)
    
    # Convert to file:// URL
    if platform.system() == 'Windows':
        url = 'file:///' + abs_path.replace('\\', '/')
    else:
        url = 'file://' + abs_path
    
    # Open the HTML file in the default browser
    webbrowser.open(url)
    
    # Print instructions for saving as PDF
    print("\n=== Instructions for saving as PDF ===")
    print("1. The HTML file has been opened in your default browser.")
    print("2. Press Ctrl+P (or Command+P on Mac) to open the print dialog.")
    print("3. Select 'Save as PDF' or 'Microsoft Print to PDF' as the printer.")
    print("4. Click 'Save' or 'Print' to save the document as a PDF.")
    print("5. Save the PDF in the Documentation folder with the same name as the HTML file.")
    print("=== End of instructions ===\n")

if __name__ == "__main__":
    html_file = "Documentation/STM32H573I-DK_Migration_Roadmap.html"
    
    # Check if the HTML file exists
    if not os.path.exists(html_file):
        print(f"Error: HTML file '{html_file}' not found.")
        print("Please run convert_md_to_html.py first to generate the HTML file.")
        exit(1)
    
    # Open the HTML file in the default browser
    open_html_file(html_file)
    
    # On Windows, also open the Documentation folder to make it easier to find the saved PDF
    if platform.system() == 'Windows':
        subprocess.Popen(['explorer', os.path.abspath('Documentation')])
