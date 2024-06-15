@echo off

echo You are going to install a docker image and run a container. Please, be patient! Wait until you see the success phrase.

docker pull existq/main:seller_bot
if %errorlevel% neq 0 (
    echo Failed to pull Docker image.
    echo ERROR OCCURED Firstly install Docker Desktop on your PC!
    exit /b 1
)

docker run -d -p 5432:5432 --name seller_bot existq/main:seller_bot
if %errorlevel% neq 0 (
    echo Failed to run Docker container.
    echo ERROR OCCURED Firstly install Docker Desktop on your PC!
    exit /b 1
)

echo Docker container 'seller_bot' is up and running on port 5432
pause
