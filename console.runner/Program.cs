using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net.Sockets;
using CommandLine;

namespace ConsoleRunner
{
    internal class Options
    {
        [Option('h', "host", Required = false, HelpText = "Host name of the test server.", Default = "ecovpn.dyndns.org")]
        public string HostName { get; set; }

        [Option('p', "port", Required = false, HelpText = "Port number on which the test server is listening.", Default = 11222)]
        public int Port { get; set; }

        [Option('s', "silent", Required = false, HelpText = "Silent mode disables writing console output.", Default = false)]
        public bool Silent { get; set; }

        [Option('m', "mapseed", Required = true, HelpText = "Map seed to pass to C++ executable. There is no default value, you must pass something.")]
        public int Seed { get; set; }

        [Option('e', "exec", Required = false, HelpText = "Path to the C++ executable.", Default = "./Wololo.exe")]
        public string ExecutablePath { get; set; }
    }

    internal class Program
    {
        private const int BUFFER_SIZE = 4096;

        private static int Main(string[] args)
        {
            return Parser.Default.ParseArguments<Options>(args)
                .WithParsed(RunOptionsAndReturnExitCode)
                .WithNotParsed(HandleParseError).Tag == ParserResultType.Parsed ? 0 : 1;
        }

        private static void HandleParseError(IEnumerable<Error> errors)
        {
            PrintUsage();
        }

        private static void RunOptionsAndReturnExitCode(Options options)
        {
            if (!File.Exists(options.ExecutablePath))
            {
                Console.WriteLine("Executable not found at: " + Path.GetFullPath(options.ExecutablePath));
                PrintUsage();
                return;
            }

            var pi = new ProcessStartInfo(options.ExecutablePath)
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardInput = true
            };
            if (options.Silent)
            {
                pi.CreateNoWindow = true;
                pi.WindowStyle = ProcessWindowStyle.Hidden;
            }

            pi.Arguments = options.Seed.ToString();

            using (var client = new TcpClient(options.HostName, options.Port))
            using (var p = Process.Start(pi))
            {
                if (null == p)
                {
                    Console.WriteLine("Could not start executable: " + options.ExecutablePath);
                    return;
                }

                Console.WriteLine("Starting game...");
                var charBuffer = new char[BUFFER_SIZE];
                var byteBuffer = new byte[sizeof(char) * BUFFER_SIZE];
                var count = -1;
                while (count != 0)
                {
                    while ((count = p.StandardOutput.Read(charBuffer, 0, charBuffer.Length)) > 0)
                    {
                        var bytes = p.StandardOutput.CurrentEncoding.GetBytes(charBuffer, 0, count);
                        client.Client.Send(bytes);
                        if (count < charBuffer.Length)
                            break;
                    }

                    while ((count = client.Client.Receive(byteBuffer)) > 0)
                    {
                        var chars = p.StandardInput.Encoding.GetChars(byteBuffer);
                        p.StandardInput.Write(chars, 0, count);
                        if (count < byteBuffer.Length)
                            break;
                    }
                }

                Console.WriteLine("DONE!");
            }
        }

        private static void PrintUsage()
        {
            Console.WriteLine("Usage:\n\tConsoleRunner.exe [--exec PATH] [--host HOSTNAME] [--port PORT_NUMBER]");
            Console.WriteLine("Use --help (or -h) switch for more details!");
            Console.WriteLine();
        }
    }
}
