import markdown
import os
import re

def convert_markdown_to_html(markdown_file, html_file):
    # Read the Markdown file
    with open(markdown_file, 'r', encoding='utf-8') as f:
        markdown_content = f.read()
    
    # Process mermaid diagrams before markdown conversion
    # Find all mermaid code blocks
    mermaid_blocks = re.findall(r'```mermaid\n(.*?)\n```', markdown_content, re.DOTALL)
    
    # Replace mermaid code blocks with placeholders
    for i, block in enumerate(mermaid_blocks):
        placeholder = f'MERMAID_PLACEHOLDER_{i}'
        markdown_content = markdown_content.replace(f'```mermaid\n{block}\n```', placeholder)
    
    # Convert Markdown to HTML
    html_content = markdown.markdown(
        markdown_content,
        extensions=['tables', 'fenced_code', 'codehilite']
    )
    
    # Replace placeholders with mermaid div elements
    for i, block in enumerate(mermaid_blocks):
        placeholder = f'MERMAID_PLACEHOLDER_{i}'
        mermaid_div = f'<div class="mermaid">\n{block}\n</div>'
        html_content = html_content.replace(placeholder, mermaid_div)
    
    # Add some basic styling
    css = '''
        body {
            font-family: Arial, sans-serif;
            line-height: 1.6;
            margin: 40px;
            max-width: 900px;
            margin-left: auto;
            margin-right: auto;
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
        .mermaid {
            text-align: center;
        }
    '''
    
    # Wrap the HTML content in a proper HTML document
    full_html = f'''
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <title>{os.path.basename(markdown_file)}</title>
        <style>
            {css}
        </style>
        <script src="https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js"></script>
        <script>
            document.addEventListener("DOMContentLoaded", function() {{
                mermaid.initialize({{ startOnLoad: true }});
            }});
        </script>
    </head>
    <body>
        {html_content}
    </body>
    </html>
    '''
    
    # Write the HTML to a file
    with open(html_file, 'w', encoding='utf-8') as f:
        f.write(full_html)
    
    print(f"Successfully converted {markdown_file} to {html_file}")
    print(f"You can open this HTML file in a browser and use the browser's print functionality to save it as a PDF.")

if __name__ == "__main__":
    markdown_file = "Documentation/STM32H573I-DK_Migration_Roadmap.md"
    html_file = "Documentation/STM32H573I-DK_Migration_Roadmap.html"
    convert_markdown_to_html(markdown_file, html_file)
