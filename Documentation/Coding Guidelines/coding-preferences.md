Core Principles

    Always prefer simple, elegant solutions (KISS principle).

    Avoid duplication of code (DRY principle); check existing codebase first.

    Only add functionality when explicitly needed (YAGNI principle).

    Adhere to SOLID principles where applicable (e.g., single responsibility, dependency inversion).

    Keep code clean, organized, and under 200-300 lines per file; refactor proactively.

Implementation Guidelines

    YOU WILL ALWAYS USE POWERSHELL SYNTAX, WE ARE ON WINDOWS 11 AND DON'T HAVE LINUX TERMINAL!!

    Write code that respects dev, test, and prod environments.

    !You never mock data for dev or prod—only for tests.

    !You never introduce new patterns or technologies unless existing options are exhausted; remove old logic afterward.

    !You never overwrite .env without my explicit confirmation.

Quality and Documentation

    After each major feature, generate a brief markdown doc in /documentation/<feature>_feature.md and add a brief summary bullet of the change/action in /Documentation/changelog.md. Also add a bullet of test results at the end of each task.

Optimize your outputs to minimize token usage while retaining clarity.
