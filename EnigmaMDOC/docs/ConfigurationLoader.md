# Config <sub>*struct*</sub>
The configurations for the MarkDown document generator
## Variables
| Name          | Type                              | Description                                                       |
|---------------|-----------------------------------|-------------------------------------------------------------------|
| outputPath    | `std::string`                     | The folder final MarkDown documents get stored too                |
| inputPath     | `std::string`                     | The folder that the generator will read files from                |
| extensions    | `std::map<std::string, FileType>` | The file extensions that the generator will read from             |
| createJSONInt | `bool`                            | Tells the generator to output the intermedeat JSON to a file      |
| commandTag    | `std::string`                     | The tag that the generator looks for in the source code comments  |

---

