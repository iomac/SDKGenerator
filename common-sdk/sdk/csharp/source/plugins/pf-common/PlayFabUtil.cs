using System;

namespace PlayFab
{
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class Unordered : Attribute
    {
        public string SortProperty { get; set; }

        public Unordered() { }

        public Unordered(string sortProperty)
        {
            SortProperty = sortProperty;
        }
    }

    public static partial class PlayFabUtil
    {
        public static readonly string[] DefaultDateTimeFormats = { // All parseable ISO 8601 formats for DateTime.[Try]ParseExact - Lets us deserialize any legacy timestamps in one of these formats
            // These are the standard format with ISO 8601 UTC markers (T/Z)
            "yyyy-MM-ddTHH:mm:ss.FFFFFFZ",
            "yyyy-MM-ddTHH:mm:ss.FFFFZ",
            "yyyy-MM-ddTHH:mm:ss.FFFZ", // DEFAULT_UTC_OUTPUT_INDEX
            "yyyy-MM-ddTHH:mm:ss.FFZ",
            "yyyy-MM-ddTHH:mm:ssZ",

            // These are the standard format without ISO 8601 UTC markers (T/Z)
            "yyyy-MM-dd HH:mm:ss.FFFFFF",
            "yyyy-MM-dd HH:mm:ss.FFFF",
            "yyyy-MM-dd HH:mm:ss.FFF", // DEFAULT_LOCAL_OUTPUT_INDEX
            "yyyy-MM-dd HH:mm:ss.FF",
            "yyyy-MM-dd HH:mm:ss",
        };
        public const int DEFAULT_UTC_OUTPUT_INDEX = 2; // The default format everybody should use
        public const int DEFAULT_LOCAL_OUTPUT_INDEX = 7; // The default format if you want to use local time (This doesn't have universal support in all PlayFab code)

        public static string GenerateErrorReport(PlayFabError error)
        {
            if (error == null)
                return null;
            return error.GenerateErrorReport();
        }
    }
}