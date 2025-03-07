import markdown
import os
from weasyprint import HTML, CSS

def convert_markdown_to_pdf(markdown_file, pdf_file):
    # Read the Markdown file
    with open(markdown_file, 'r', encoding='utf-8') as f:
        markdown_content = f.read()
    
    # Convert Markdown to HTML
    html_content = markdown.markdown(
        markdown_content,
        extensions=['tables', 'fenced_code', 'codehilite']
    )
    
    # Add some basic styling
    css = CSS(string='''
        body {
            font-family: Arial, sans-serif;
            line-height: 1.6;
            margin: 40px;
        }
        h1, h2, h3, h4, h5, h6 {
            color: #333;
            margin-top: 20px;
        }
        code {
            background-color: #f5f5f5;
            padding: 2px 4px;
            border-radius: 4px;
            font-family: monospace;
        }
        pre {
            background-color: #f5f5f5;
            padding: 10px;
            border-radius: 4px;
            overflow-x: auto;
        }
        table {
            border-collapse: collapse;
            width: 100%;
        }
        th, td {
            border: 1px solid #ddd;
            padding: 8px;
        }
        th {
            background-color: #f2f2f2;
        }
        blockquote {
            border-left: 4px solid #ddd;
            padding-left: 10px;
            color: #666;
        }
    ''')
    
    # Wrap the HTML content in a proper HTML document
    full_html = f'''
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <title>{os.path.basename(markdown_file)}</title>
    </head>
    <body>
        {html_content}
    </body>
    </html>
    '''
    
    # Convert HTML to PDF
    HTML(string=full_html).write_pdf(pdf_file, stylesheets=[css])
    
    print(f"Successfully converted {markdown_file} to {pdf_file}")

if __name__ == "__main__":
    markdown_file = "Documentation/STM32H573I-DK_Migration_Roadmap.md"
    pdf_file = "Documentation/STM32H573I-DK_Migration_Roadmap.pdf"
    convert_markdown_to_pdf(markdown_file, pdf_file)
