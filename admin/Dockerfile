FROM python:3.12-slim-bookworm

# Prevent Python from writing .pyc files and enable unbuffered output
ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

# Install system dependencies for Pillow
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        libjpeg62-turbo-dev \
        zlib1g-dev \
        libfreetype6-dev && \
    rm -rf /var/lib/apt/lists/*

# Create non-root user with UID 1000 (matches typical 'mud' user)
RUN groupadd -g 1000 admin && \
    useradd -u 1000 -g admin -m -s /bin/sh admin

WORKDIR /app

# Install Python dependencies
COPY admin/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
COPY admin/*.py ./
COPY admin/templates/ ./templates/
COPY admin/static/ ./static/

# Create writable directories
RUN mkdir -p /backups /admin-data && \
    chown -R admin:admin /app /backups /admin-data

USER admin

EXPOSE 8080

CMD ["gunicorn", "--bind", "0.0.0.0:8080", "--workers", "1", "--timeout", "300", "app:create_app()"]
