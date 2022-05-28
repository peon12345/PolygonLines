#include <QCoreApplication>
#include <cmath>
#include <optional>
#include <fstream>
#include <QDir>
#include <sstream>
#include <iostream>

enum class LineStatus : uint8_t {
  INSIDE = 0,
  INTERSECT = 1,
  PARTIALLY_INTERSECT = 2,
  OUTSIDE = 3
};

struct Point final{
  float x = NAN,y = NAN;

  bool isValid() const{
    return(!std::isnan(x) &&  !std::isnan(y));
  }
};


class Line final{
public:
  Line(){}
  Line(const Point& point1 ,const Point& point2) : m_point1(point1) , m_point2(point2){}

  const Point& point1() const{
    return m_point1;
  }

  const Point& point2() const{
    return m_point2;
  }

  void setPoint1(const Point& point){
    m_point1 = point;
  }

  void setPoint2(const Point& point){
    m_point2 = point;
  }

  bool intersect(const Line& anotherLine) const{

    //    Отрезки пересекаются когда:
    //   1)Пересекаются прямоугольники, которые их ограничивают;
    //   2)[AC x AB]·[AD x AB] ≤ 0;
    //   3)[CA x CD]·[CB x CD] ≤ 0.


    if(!rectangleIntersect(anotherLine) ) {
        return false;
      }

    //A - начальная точка
    //B - конечная точка

    //C - начальная точка другого отрезка
    //D - конечная точка другого отрезка

    //координаты вектора
    float ABx, ABy, ACx, ACy, ADx, ADy;
    float CAx, CAy, CBx, CBy, CDx, CDy;


    float ACxAB, ADxAB, CAxCD, CBxCD;

    ACx = anotherLine.m_point1.x - m_point1.x; ACy = anotherLine.m_point1.y - m_point1.y;

    ABx = m_point2.x - m_point1.x; ABy = m_point2.y - m_point1.y;

    ADx = anotherLine.m_point2.x - m_point1.x; ADy = anotherLine.m_point2.y - m_point1.y;

    CAx = m_point1.x - anotherLine.m_point1.x; CAy = m_point1.y - anotherLine.m_point1.y;

    CBx = m_point2.x - anotherLine.m_point1.x; CBy = m_point2.y - anotherLine.m_point1.y;

    CDx = anotherLine.m_point2.x - anotherLine.m_point1.x; CDy = anotherLine.m_point2.y - anotherLine.m_point1.y ;

    ACxAB = ACx * ABy - ACy * ABx;

    ADxAB = ADx * ABy - ADy * ABx;

    CAxCD = CAx * CDy - CAy * CDx;

    CBxCD = CBx * CDy - CBy * CDx;

    return ACxAB * ADxAB <= 0 && CAxCD * CBxCD <= 0;

  }

  bool isValid()const{

    return (m_point1.isValid() && m_point2.isValid());
  }

private:

  bool rectangleIntersect(const Line& anotherLine) const{

    //создаем ограничивающий прямоугольник для отрезка ( bounding rect)
    float x1 = std::min(m_point1.x ,m_point2.x );
    float y1 = std::min(m_point1.y ,m_point2.y);
    float x2 = std::max(m_point1.x ,m_point2.x );
    float y2 = std::max(m_point1.y ,m_point2.y);

    float x3 = std::min(anotherLine.m_point1.x ,anotherLine.m_point2.x );
    float y3 = std::min(anotherLine.m_point1.y ,anotherLine.m_point2.y );
    float x4 = std::max(anotherLine.m_point1.x ,anotherLine.m_point2.x );
    float y4 = std::max(anotherLine.m_point1.y ,anotherLine.m_point2.y );


    //проверка на пересечение
    if ((x3 - x2)*(x4 - x1) > 0 ||

        ((y3 - y2)*(y4 - y1) > 0)){
        return 0;
      }

    return 1;
  }

private:
  Point m_point1;
  Point m_point2;
};



class Polygon final{
public:
  Polygon() = default;
  Polygon(std::vector<Point>&& vertices) : m_vertices(std::move(vertices)) {}

  void addPoint(const Point& point){
    if(point.isValid())
      m_vertices.push_back(point);
  }

  std::optional<LineStatus> intersectStatus(const Line& anotherLine) const {

    if(!isValid()){
        return std::nullopt;
      }


    std::vector<Line> lines;
    lines.reserve(m_vertices.size());

    float maxY = 0;
    float maxX = 0;

    Line lineTemp;
    for(const Point& point : m_vertices){

        if(!lineTemp.isValid()){
            lineTemp.setPoint1(m_vertices.back());
            lineTemp.setPoint2(point);

          }else{
            lineTemp.setPoint1(lineTemp.point2());
            lineTemp.setPoint2(point);
          }

        lines.push_back(lineTemp);

        if(point.x > maxX)
          maxX = point.x;

        if(point.y > maxY)
          maxY = point.y;

      }

    uint counterIntersect = 0;
    for(const Line& line : lines){

        if(line.intersect(anotherLine) ){ // пересекает ли отрезок многоугольник
            counterIntersect++;
          }
      }

    if(counterIntersect % 2 != 0){ // если кол-во пересечений нечет , то одна точка снаружи
        return LineStatus::PARTIALLY_INTERSECT;
      }

    else if(counterIntersect % 2 == 0 || counterIntersect == 0) { // если кол-во пересечений чет , точки могут быть обе снаружи или обе внутри
        //если кол-во пересечений == 0 , точки могут быть внутри многоугольника.

        //нужно посчитать сколько пересечений делает отрезок до правой верхней точки bounding rect,узнаем четное оно или нет.
        Point topRightCornerBoundingRect{maxX,maxY};
        Line lineLikeRay (anotherLine);
        lineLikeRay.setPoint2(topRightCornerBoundingRect);

        uint counterIntersectForRay = 0;
        for(const Line& line : lines){

            if(line.intersect(lineLikeRay) ){
                counterIntersectForRay++;
              }
          }

        if(counterIntersectForRay % 2 == 0 || counterIntersectForRay == 0)

          if(counterIntersect > 0)  // если у нас до этого (для отрезка) были пересечения, то отрезок пересекает многоугольник или внутри многоугольника.
            return LineStatus::INTERSECT;
          else
            return LineStatus::OUTSIDE;

        else if(counterIntersectForRay % 2 != 0)
          return LineStatus::INSIDE;
      }


    return LineStatus::OUTSIDE;
  }

  bool isValid() const{

    if(m_vertices.size() < 3)
      return false;

    return true;

  }
private:
  std::vector<Point> m_vertices;
};


std::vector<Point> readPoints(const std::string& path){

  static constexpr char delimeterXY = ',';
  static constexpr char delimeterPoints = ' ';

  std::vector<Point> resultPoints;

  std::ifstream in(path);

  if (in.is_open())
    {
      std::string line;
      while(getline(in,line)){

          std::string XY;
          std::istringstream tokenStream(line);

          while (std::getline(tokenStream, XY, delimeterPoints))
            {
              size_t pos = 0;
              float x;
              float y;
              pos = XY.find(delimeterXY);

              if( pos != std::string::npos) {
                  x = std::stof(XY.substr(0, pos));
                  XY.erase(0, pos + 1);
                  y = std::stof(XY);

                  resultPoints.emplace_back(Point{x,y});
                }
            }
        }
    }

  return resultPoints;
}


std::string descriptionStatus(LineStatus status){

  switch (status) {

    case LineStatus::INSIDE:
      return "Inside the polygon.";


    case LineStatus::OUTSIDE:
      return "Doesn't intersect polygon.";

    case LineStatus::INTERSECT:
      return "Intersects the polygon.";


    case LineStatus::PARTIALLY_INTERSECT:
      return "Partially intersects the polygon.";

    default:
      return "";
    }
}

bool saveStatuses(const std::string& path,const std::vector<std::optional<LineStatus>>& statuses) {

  std::ofstream out;
  out.open(path);
  if (out.is_open())
    {
      for(std::optional<LineStatus> status : statuses){
          if(status)
            out << std::to_string(static_cast<uint8_t>(status.value())) << " " << descriptionStatus(status.value()) << std::endl;
          else
            out << "Error.";
        }

      return true;
    }
  return false;
}



void printLineStatus( const Line& line ,std::optional<LineStatus> status){

  std::string lineStr = "Line :" + std::to_string(line.point1().x) + "," + std::to_string(line.point1().y) +
      " " + std::to_string(line.point2().x) + "," + std::to_string(line.point2().y);
  if(status)
    std::cout << lineStr << " " << descriptionStatus(status.value());
  else
    std::cout << lineStr << "Error.";

  std::cout << std::endl;
}


std::vector<std::optional<LineStatus>> getStatuses(const Polygon& polygon , const std::vector<Line>& lines){

  std::vector<std::optional<LineStatus>> resultStatus;
  resultStatus.reserve(lines.size());

  for(const Line& line : lines){
      std::optional<LineStatus> status = polygon.intersectStatus(line);

      resultStatus.push_back(status);
      printLineStatus(line,status);
    }

  return resultStatus;
}


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  static const std::string currentDir = QDir::currentPath().toStdString() + QDir::separator().toLatin1();
  static const std::string pathPolygon = currentDir +  "polys.txt";
  static const std::string pathLines = currentDir +  "lines.txt";
  static const std::string pathSaveStatuses  = currentDir + "results.txt";

  Polygon polygon = readPoints(pathPolygon);

  std::vector<Point> pointLines = readPoints(pathLines);

  int sizePoint = pointLines.size();
  if(sizePoint % 2 != 0){
      std::cout << "error read lines";
      return 0;
    }

  std::vector<Line> lines;
  lines.reserve(sizePoint/2);

  for(auto it = pointLines.cbegin(), itEnd = pointLines.cend(); it < itEnd; it += 2 ){
      lines.push_back(Line(*it,*(it+1)));
    }

  std::vector<std::optional<LineStatus>> resultStatus = getStatuses(polygon,lines);

  if( saveStatuses(pathSaveStatuses,resultStatus)) {

      std::cout << "Result saved. Directory:" << pathSaveStatuses;
    }else{
      std::cout << "Result not saved, check path:" << std::endl << pathSaveStatuses;
    }

  return a.exec();
}







