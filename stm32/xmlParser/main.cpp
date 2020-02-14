#include <stdio.h>
#include <QXmlStreamReader>
#include <QFile>
#include <QIODevice>


typedef enum {
    SRC_MODE = 0,
    HDR_MODE,
    DEF_MODE
} s_cxema_t;

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

static void prn_str(const QString & stx, bool prn)
{
    if (prn) {
        QByteArray tmp;
        tmp.append(stx);
        fprintf(stdout, "%s\n", tmp.data());
    }
}
//---------------------------------------------------------------------------
void checkSym(QByteArray *arr)
{
    QByteArray tmp(*arr);
    if ((tmp.indexOf('"', 0) != -1) && (tmp.indexOf('"', tmp.length() - 1) != -1)) {
        tmp.remove(tmp.length() - 1, 1);
        int len = tmp.length();
        tmp = tmp.mid(1, len - 1);
        if (tmp.length() <= arr->length()) *arr = tmp;
    }
}

//---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
QString fileName;
QString outName("sdklink.src");
bool setPrint = true;
s_cxema_t cxema = SRC_MODE;

    setlocale(LC_ALL,"UTF8");

    if (argc > 1) fileName.append(argv[1]);
    else {
        prn_str("[xmlParser] No xml file. Bye.", setPrint);
        return 1;
    }
    if (argc > 2) {
        if (!strcmp(argv[2], "hdr")) cxema = HDR_MODE;
        else
        if (!strcmp(argv[2], "def")) cxema = DEF_MODE;
    }
    if (argc > 3) {
        if (!strcmp(argv[3], "silent")) setPrint = false;
    }

    if (cxema == HDR_MODE) outName = "sdklink.hdr";
    else
    if (cxema == DEF_MODE) outName = "sdklink.def";

    QXmlStreamReader *xmlReader = nullptr;
    QFile *xmlFile = nullptr;
    QFile *outFile = nullptr;

    try {
        //open xml file for reading project-file (.project)
        xmlFile = new QFile(fileName);
        if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
            prn_str("[xmlParser] Couldn't open source file " + fileName + " . Bye", setPrint);
            return 1;
        }
        //open output file
        outFile = new QFile(outName);
        if (!outFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            prn_str("[xmlParser] Couldn't open source file " + outName + " . Bye", setPrint);
            xmlFile->close();
            return 1;
        }

        xmlReader = new QXmlStreamReader(xmlFile);
        if (!xmlReader) {
            xmlFile->close();
            outFile->close();
            return 1;
        }
        QXmlStreamReader::TokenType token;
        QByteArray line;
        uint32_t Counter = 0;
        QString nameProject = "";
        bool labelFlag = false;
        bool linkFlag = false;
        const QString tag_hdr("projectDescription");
        const QString tag_loc("location");
        const QString tag_name("name");
        const QString tag_type("type");
        const QString tag_link("linkedResources");

        const QString tag_lng("language");
        const QString tag_inc("includepath");
        bool done = false;

        const QString tag_mac("macro");
        const QString tag_val("value");
        bool macFlag = false;

        while (!xmlReader->atEnd() && !xmlReader->hasError()) {
            token = xmlReader->readNext();
            if (token == QXmlStreamReader::StartDocument) continue;
            if (token == QXmlStreamReader::StartElement) {
                switch (cxema) {
                    case SRC_MODE:
                        //-----------------------------------------------------------------------------------------------------------------
                        if (xmlReader->name() == tag_hdr) {
                            labelFlag = true;
                            //continue;
                        } else if (xmlReader->name() == tag_link) {
                            linkFlag = true;
                        } else if (xmlReader->name() == tag_name) {
                            if (labelFlag) {
                                labelFlag = false;
                                if (!nameProject.length()) {
                                    nameProject.append(xmlReader->readElementText());
                                    prn_str("ProjectName:" + nameProject, setPrint);
                                    //line.clear();
                                    //line.append("#" + nameProject + "\n");
                                    //outFile->write(line);
                                }
                            } else {
                                if (linkFlag) {
                                    prn_str("Elem[" + QString::number(++Counter, 10) + "]\n\t" + xmlReader->name() + " : " + line, setPrint);
                                }
                            }
                        } else if(xmlReader->name() == tag_type) {
                            //continue;
                        } else if(xmlReader->name() == tag_loc) {
                            line.clear();
                            line.append(xmlReader->readElementText());
                            if (line[line.length() - 2] == '.') line.remove(line.length() - 2, 2);//remove ".c"
                            prn_str("\t" + xmlReader->name() + " : " + line, setPrint);
                            line.append("\n");
                            outFile->write(line);
                        }
                        //-----------------------------------------------------------------------------------------------------------------
                    break;
                    case HDR_MODE:
                        //-----------------------------------------------------------------------------------------------------------------
                        if (xmlReader->name() == tag_lng) {
                            if (!labelFlag) {
                                labelFlag = true;
                            } else {
                                done = true;
                                break;
                            }
                        } else if (xmlReader->name() == tag_inc) {
                            if (labelFlag) {
                                line.clear();
                                line.append(xmlReader->readElementText());
                                prn_str("\t" + xmlReader->name() + " : " + line, setPrint);
                                line.append("\n");
                                outFile->write(line);
                            }
                        }
                        //-----------------------------------------------------------------------------------------------------------------
                    break;
                    case DEF_MODE:
                        //-----------------------------------------------------------------------------------------------------------------
                        if (xmlReader->name() == tag_lng) {
                            if (macFlag) {
                                done = true;
                                break;
                            }
                        } else if (xmlReader->name() == tag_mac) {
                            labelFlag = true;
                        } else if (xmlReader->name() == tag_name) {
                            if (labelFlag) {
                                line.clear();
                                line.append(xmlReader->readElementText());
                                prn_str("\t" + xmlReader->name() + " : " + line, setPrint);
                                //line.append("\n");
                                outFile->write(line);
                                macFlag = true;
                            }
                        } else if (xmlReader->name() == tag_val) {
                            if (labelFlag) {
                                labelFlag = false;
                                line.clear();
                                line.append(xmlReader->readElementText());
                                checkSym(&line);
                                prn_str("\t" + xmlReader->name() + " : " + line, setPrint);
                                if (line.length()) line.insert(0, "=");
                                line.append("\n");
                                outFile->write(line);
                            }
                        }
                        //-----------------------------------------------------------------------------------------------------------------
                    break;
                }
                if (done) break;
            }
        }
        //
        if (xmlReader->hasError()) {
            prn_str("[xmlParser] xmlFile.xml Parse Error : " + xmlReader->errorString(), setPrint);
            xmlReader->clear();
            xmlFile->close();
            outFile->close();
            return 1;
        }
        //
        xmlReader->clear();
        xmlFile->close();
        outFile->close();
        //
     }
     catch (bad_alloc) {
        perror("[xmlParser] Error while alloc memory\n");
        return -1;
     }

     return 0;
}
