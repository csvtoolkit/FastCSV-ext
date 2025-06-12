# Contributing to FastCSV-ext

We love your input! We want to make contributing to FastCSV as easy and transparent as possible, whether it's:

- Reporting a bug
- Discussing the current state of the code
- Submitting a fix
- Proposing new features
- Becoming a maintainer

## Development Process

We use GitHub to host code, to track issues and feature requests, as well as accept pull requests.

1. Fork the repo and create your branch from `master`.
2. If you've added code that should be tested, add tests.
3. If you've changed APIs, update the documentation.
4. Ensure the test suite passes.
5. Make sure your code follows the existing style.
6. Issue that pull request!

## Any contributions you make will be under the MIT Software License

In short, when you submit code changes, your submissions are understood to be under the same [MIT License](LICENSE) that covers the project. Feel free to contact the maintainers if that's a concern.

## Report bugs using GitHub's [issue tracker](https://github.com/csvtoolkit/FastCSV-ext/issues)

We use GitHub issues to track public bugs. Report a bug by [opening a new issue](https://github.com/csvtoolkit/FastCSV-ext/issues/new); it's that easy!

## Write bug reports with detail, background, and sample code

**Great Bug Reports** tend to have:

- A quick summary and/or background
- Steps to reproduce
  - Be specific!
  - Give sample code if you can.
- What you expected would happen
- What actually happens
- Notes (possibly including why you think this might be happening, or stuff you tried that didn't work)

## Development Setup

1. Install PHP development headers and tools:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install php-dev

   # CentOS/RHEL
   sudo yum install php-devel

   # macOS
   brew install php
   ```

2. Clone your fork:
   ```bash
   git clone https://github.com/csvtoolkit/FastCSV-ext.git
   cd fastcsv
   ```

3. Build the extension:
   ```bash
   phpize
   ./configure
   make
   ```

4. Run tests:
   ```bash
   make test
   ```

## Testing

We use PHP's built-in testing framework (phpt). Please add relevant tests for any new features or bug fixes.

Test files are located in the `tests/` directory. Each test file should:
- Have a clear name indicating what it tests
- Include test cases for both success and failure scenarios
- Follow the existing test format

## Coding Style

- Follow the existing code style
- Use meaningful variable and function names
- Comment complex logic
- Keep functions focused and concise
- Use consistent indentation (4 spaces)
- Add PHPDoc blocks for public methods

## License

By contributing, you agree that your contributions will be licensed under its MIT License. 