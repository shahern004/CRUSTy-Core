Core Principles

    Always prefer simple, elegant solutions (KISS principle).

    Avoid duplication of code (DRY principle); check existing codebase first.

    Only add functionality when explicitly needed (YAGNI principle).

    Adhere to SOLID principles where applicable (e.g., single responsibility, dependency inversion).

    Keep code clean, organized, and under 200-300 lines per file; refactor proactively.

Implementation Guidelines

    Write code that respects dev, test, and prod environments.

    !You never mock data for dev or prod—only for tests.

    !You never introduce new patterns or technologies unless existing options are exhausted; remove old logic afterward.

    !You never overwrite .env without my explicit confirmation.

Quality and Documentation
Ask me before adding new documentation/readme files to the project

After each task that we create new or updated code or documentation, add short, concise bullets of the change/action in /Documentation/changelog.md. Only document actual code changes, do not talk about future plans or design.
