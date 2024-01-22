
#include"Polygen.h"


std::pair<int,int> makeEdges(const std::vector<VertexData>& vertices, std::unordered_map<int, std::list<Edge> >& edge_table, int id) { //���㷽������ʱ���
	//������趼����������Ƭ
	int ymax = INT_MIN, ymin = INT_MAX;
	int n = vertices.size();
	//dx: ������Ļ���Y->Y+1ʱ�� ��Ļ��X�ı仯��
	//��֪Edge �����˵����Ļ����(X1,Y1) (X2,Y2)
	//dx����ֱ�Ӽ������
	auto prev = (vertices[n - 1].spos.y - vertices[0].spos.y);
	for (int i = 0; i < n; i++) {
		//ÿ�ι���vertices[i] -> vertices[i+1]������ p1p2
		auto p1 = vertices[i].spos;
		auto p2 = vertices[(i + 1) % n].spos;
		auto p0 = vertices[(i - 1 + n) % n].spos;
		//�ȼ���ߵ�dx
		Edge e;
		int y01 = p0.y - p1.y;
		int y21 = p2.y - p1.y;
		if (y21 == 0) continue; //������ƽ����x��ı�
		//NDC�ռ�����
		glm::vec4 cpos1 = vertices[i].cpos;
		glm::vec4 cpos2 = vertices[(i + 1) % n].cpos;
		double dx = -(p2.x - p1.x) * 1.0 / y21;
		if (y01 * y21 < 0) {
			//���Ǽ�ֵ��
			if (p1.y > p2.y) { //yi���϶˵�, dy��yi����Ҫ-1
				ymax = std::max(ymax, p1.y - 1);
				ymin = std::min(ymin, p2.y - 1);
				e.cpos = cpos1;
				e.dx = dx; e.dy = p1.y - p2.y - 1; e.id = id;
				e.x = (p1.x + dx);
				edge_table[p1.y-1].push_back(e);
			}
			else { // yi���¶˵㣬yi����, dy-1
				ymin = std::min(ymin, p1.y + 1);
				e.cpos = cpos2;
				e.dx = dx; e.dy = p2.y - p1.y - 1; e.id = id;
				e.x = (p2.x);
				edge_table[p2.y].push_back(e);
			}
		}
		else { //�Ǽ�ֵ�㣬���ö��⴦��
			ymin = std::min(ymin, std::min(p1.y, p2.y));
			ymax = std::max(ymax, std::max(p1.y, p2.y));
			if (p1.y < p2.y) {
				glm::ivec2 tmp = p1;
				p1 = p2;
				p2 = tmp;
				glm::vec4 tmpc = cpos1;
				cpos1 = cpos2;
				cpos2 = tmpc;
			}
			e.cpos = cpos1;
			e.dx = dx; e.dy = p1.y - p2.y; e.id = id;
			e.x = (p1.x);
			edge_table[p1.y].push_back(e);
		}
	}
	return { ymax, ymin };
	//return max_y;
}