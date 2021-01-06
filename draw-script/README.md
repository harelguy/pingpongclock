# Ping Pong Clock - Web Application
This script is the client-side web application used to control the Ping Pong Clock.

## Development
- Install dependencies
```bash
npm i
```
- Run the development server, which will simulate the clock. This is to prevent needing to install the server on the clock itself in order to test the app.
```bash
npm run server
```

- In a different terminal, run the web application
```bash
npm start
```

## Deployment
Deploy to your clock by building the asset bundle and uploading it to the SPIFS storage on the clock.
- Build the assets
```bash
npm build
```
- Open your clock's "upload file" dialog: `http://<clock-ip-address>/upload`
