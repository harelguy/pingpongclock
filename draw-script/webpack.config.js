const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const { CleanWebpackPlugin } = require('clean-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const AwesomeTypescriptLoader = require('awesome-typescript-loader');

module.exports = {
  entry: './src/index.ts',
  mode: 'development',
  devtool: 'source-map',
  watch: true,
  module: {
    rules: [
      { test: /\.tsx?$/, loader: 'awesome-typescript-loader' },
      { enforce: 'pre', test: /\.js$/, loader: 'source-map-loader', exclude: path.join(process.cwd(), 'node_modules/pts') },
      { test: /\.css$/, use: ['style-loader', 'css-loader'] }
    ],
  },
  plugins: [
    new CleanWebpackPlugin(),
    new HtmlWebpackPlugin({
      title: 'Pingpong Clock',
      template: './src/index.html'
    })
  ],
  output: {
    filename: 'bundle.js',
    path: path.resolve(__dirname, '../data'),
  },
  resolve: {
    extensions: ['.ts', '.js', '.json'],
  },
  devServer: {
    host: '0.0.0.0'
  }
};
