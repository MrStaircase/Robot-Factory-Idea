```mermaid
flowchart TD
    A[Start] --> B[Save data]
    B --> C{Success?}
    C -->|Yes| D[Finish]
    C -->|No| E[Show error]

    click B "https://example.com/save-docs" "Read save documentation"
```