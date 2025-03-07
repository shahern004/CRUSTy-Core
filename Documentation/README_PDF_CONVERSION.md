# PDF Conversion Guide for CRUSTy-Core Documentation

This guide explains how to convert Markdown documentation files to HTML and PDF formats.

## Prerequisites

- Python 3.6 or higher
- A modern web browser (Chrome, Firefox, Edge, etc.)

## Available Scripts

The repository includes the following scripts for converting documentation:

1. `convert_md_to_html.py` - Converts Markdown files to HTML with proper styling and mermaid diagram support
2. `open_html_for_pdf.py` - Opens the HTML file in a browser and provides instructions for saving as PDF

## Converting Markdown to HTML

To convert a Markdown file to HTML:

```bash
python convert_md_to_html.py
```

This script will:

- Read the Markdown file (`Documentation/STM32H573I-DK_Migration_Roadmap.md`)
- Convert it to HTML with proper styling
- Process mermaid diagrams to ensure they render correctly
- Save the result as `Documentation/STM32H573I-DK_Migration_Roadmap.html`

## Converting HTML to PDF

To convert the HTML file to PDF:

```bash
python open_html_for_pdf.py
```

This script will:

- Open the HTML file in your default browser
- Provide instructions for saving it as a PDF
- On Windows, also open the Documentation folder for easy access

### Manual PDF Conversion Steps

1. When the HTML file opens in your browser, press `Ctrl+P` (or `Command+P` on Mac) to open the print dialog
2. Select "Save as PDF" or "Microsoft Print to PDF" as the printer
3. Adjust settings as needed (orientation, margins, etc.)
4. Click "Save" or "Print" to save the document as a PDF
5. Save the PDF in the Documentation folder with the same name as the HTML file

## Customizing the Conversion

If you need to convert different Markdown files or customize the output:

1. Edit `convert_md_to_html.py` and modify:

   - The input Markdown file path
   - The output HTML file path
   - CSS styling for the HTML output

2. Edit `open_html_for_pdf.py` and modify:
   - The HTML file path to open

## Troubleshooting

- **Mermaid diagrams not rendering**: Make sure your browser has JavaScript enabled
- **HTML file not found**: Run `convert_md_to_html.py` first to generate the HTML file
- **PDF quality issues**: Adjust the print settings in your browser (e.g., enable background graphics)
